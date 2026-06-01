#pragma once

#include <CS2/SDK/CSchemaOffset.hpp>

namespace CookieCore
{
	// Thin wrapper over existing CShcemaOffset — same role as reference SCHEMA/PSCHEMA macros.
	inline auto GetSchemaFieldOffset( const char* className , const char* fieldName ) -> uint32_t
	{
		return GetSchemaOffset()->GetOffset( className , fieldName );
	}
}

#define COOKIE_SCHEMA( type , method , className , fieldName ) \
	__forceinline type& method() { \
		static const uint32_t offset = CookieCore::GetSchemaFieldOffset( className , fieldName ); \
		return *reinterpret_cast<type*>( reinterpret_cast<uintptr_t>( this ) + offset ); \
	}

#define COOKIE_PSCHEMA( type , method , className , fieldName ) \
	__forceinline type* method() { \
		static const uint32_t offset = CookieCore::GetSchemaFieldOffset( className , fieldName ); \
		return reinterpret_cast<type*>( reinterpret_cast<uintptr_t>( this ) + offset ); \
	}
