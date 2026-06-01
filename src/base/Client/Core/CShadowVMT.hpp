#pragma once

#include <Common/Common.hpp>

#include <cstddef>
#include <cstdint>

namespace CookieCore
{
	class CShadowVMT final
	{
	public:
		CShadowVMT() = default;
		explicit CShadowVMT( void* classBase );
		~CShadowVMT();

		CS_CLASS_NO_ASSIGNMENT( CShadowVMT );

		auto Setup( void* classBase = nullptr ) -> bool;

		template<typename T>
		auto HookIndex( int index , T function ) -> void
		{
			if ( !m_NewVTable )
				return;

			m_NewVTable[index + 1] = reinterpret_cast<std::uintptr_t>( function );
		}

		template<typename T>
		auto GetOriginal( int index ) -> T
		{
			if ( !m_OldVTable )
				return nullptr;

			return reinterpret_cast<T>( m_OldVTable[index] );
		}

		auto UnhookIndex( int index ) -> void;
		auto UnhookAll() -> void;

	private:
		auto CalcVTableLength( std::uintptr_t* vTableStart ) -> std::size_t;

	private:
		void* m_ClassBase = nullptr;
		std::size_t m_VTableLength = 0;
		std::uintptr_t* m_NewVTable = nullptr;
		std::uintptr_t* m_OldVTable = nullptr;
	};
}
