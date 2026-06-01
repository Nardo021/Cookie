#include "CShadowVMT.hpp"

#include <cstring>

namespace CookieCore
{
	CShadowVMT::CShadowVMT( void* classBase )
		: m_ClassBase( classBase )
	{
	}

	CShadowVMT::~CShadowVMT()
	{
		UnhookAll();
	}

	auto CShadowVMT::Setup( void* classBase ) -> bool
	{
		if ( classBase )
			m_ClassBase = classBase;

		if ( !m_ClassBase )
			return false;

		m_OldVTable = *reinterpret_cast<std::uintptr_t**>( m_ClassBase );
		if ( !m_OldVTable )
			return false;

		m_VTableLength = CalcVTableLength( m_OldVTable );
		if ( m_VTableLength == 0 )
			return false;

		m_NewVTable = new std::uintptr_t[m_VTableLength + 1]{};
		std::memcpy( &m_NewVTable[1] , m_OldVTable , m_VTableLength * sizeof( std::uintptr_t ) );

		DWORD oldProtect = 0;
		if ( !VirtualProtect( m_ClassBase , sizeof( std::uintptr_t ) , PAGE_READWRITE , &oldProtect ) )
		{
			delete[] m_NewVTable;
			m_NewVTable = nullptr;
			return false;
		}

		m_NewVTable[0] = m_OldVTable[-1];
		*reinterpret_cast<std::uintptr_t**>( m_ClassBase ) = &m_NewVTable[1];
		VirtualProtect( m_ClassBase , sizeof( std::uintptr_t ) , oldProtect , &oldProtect );

		return true;
	}

	auto CShadowVMT::UnhookIndex( int index ) -> void
	{
		if ( !m_NewVTable || !m_OldVTable )
			return;

		m_NewVTable[index + 1] = m_OldVTable[index];
	}

	auto CShadowVMT::UnhookAll() -> void
	{
		if ( !m_ClassBase || !m_OldVTable )
		{
			delete[] m_NewVTable;
			m_NewVTable = nullptr;
			m_OldVTable = nullptr;
			m_VTableLength = 0;
			return;
		}

		DWORD oldProtect = 0;
		if ( VirtualProtect( m_ClassBase , sizeof( std::uintptr_t ) , PAGE_READWRITE , &oldProtect ) )
		{
			*reinterpret_cast<std::uintptr_t**>( m_ClassBase ) = m_OldVTable;
			VirtualProtect( m_ClassBase , sizeof( std::uintptr_t ) , oldProtect , &oldProtect );
		}

		delete[] m_NewVTable;

		m_NewVTable = nullptr;
		m_OldVTable = nullptr;
		m_ClassBase = nullptr;
		m_VTableLength = 0;
	}

	auto CShadowVMT::CalcVTableLength( std::uintptr_t* vTableStart ) -> std::size_t
	{
		MEMORY_BASIC_INFORMATION memInfo{};
		std::size_t length = 0;

		while ( true )
		{
			if ( !VirtualQuery( reinterpret_cast<LPCVOID>( vTableStart[length] ) , &memInfo , sizeof( memInfo ) ) )
				break;

			if ( memInfo.Protect != PAGE_EXECUTE_READ && memInfo.Protect != PAGE_EXECUTE_READWRITE )
				break;

			++length;
		}

		return length;
	}
}
