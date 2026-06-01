#pragma once

#include <Common/Common.hpp>
#include <Common/Include/Fnv1a/Hash_Fnv1a_Constexpr.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>

namespace CookieCore
{
	struct Color4
	{
		float r = 1.f;
		float g = 1.f;
		float b = 1.f;
		float a = 1.f;
	};

	enum class EConfigValueType : std::uint8_t
	{
		Bool ,
		Int ,
		Float ,
		Color4
	};

	struct ConfigVariable_t
	{
		std::uint64_t uNameHash = 0;
		std::uint64_t uTypeHash = 0;
		EConfigValueType eType = EConfigValueType::Bool;
		bool bValue = false;
		int iValue = 0;
		float flValue = 0.f;
		Color4 colValue{};
	};

	class CCookieConfig final
	{
	public:
		static constexpr std::size_t INVALID_INDEX = static_cast<std::size_t>( -1 );

	public:
		template<typename T>
		static auto AddVariable( const std::uint64_t uNameHash , const std::uint64_t uTypeHash , const T& defaultValue ) -> std::size_t;

		template<typename T>
		static auto Get( const std::size_t nIndex ) -> T&;

		template<typename T>
		static auto Set( const std::size_t nIndex , const T& value ) -> bool;

		static auto GetVariableIndex( const std::uint64_t uNameHash ) -> std::size_t;

		static auto GetConfigDirectory() -> std::string;
		static auto Save( const std::string& fileName ) -> bool;
		static auto Load( const std::string& fileName ) -> bool;

	private:
		static auto TypeToEnum( const std::uint64_t uTypeHash ) -> EConfigValueType;
		static auto EnumToTypeHash( EConfigValueType eType ) -> std::uint64_t;

	public:
		inline static std::vector<ConfigVariable_t> vecVariables{};
	};

	template<typename T>
	auto CCookieConfig::AddVariable( const std::uint64_t uNameHash , const std::uint64_t uTypeHash , const T& defaultValue ) -> std::size_t
	{
		ConfigVariable_t entry{};
		entry.uNameHash = uNameHash;
		entry.uTypeHash = uTypeHash;

		if constexpr ( std::is_same_v<T , bool> )
		{
			entry.eType = EConfigValueType::Bool;
			entry.bValue = defaultValue;
		}
		else if constexpr ( std::is_same_v<T , int> )
		{
			entry.eType = EConfigValueType::Int;
			entry.iValue = defaultValue;
		}
		else if constexpr ( std::is_same_v<T , float> )
		{
			entry.eType = EConfigValueType::Float;
			entry.flValue = defaultValue;
		}
		else if constexpr ( std::is_same_v<T , Color4> )
		{
			entry.eType = EConfigValueType::Color4;
			entry.colValue = defaultValue;
		}
		else
		{
			static_assert( sizeof( T ) == 0 , "Unsupported config variable type" );
		}

		vecVariables.emplace_back( entry );
		return vecVariables.size() - 1U;
	}

	template<typename T>
	auto CCookieConfig::Get( const std::size_t nIndex ) -> T&
	{
		auto& entry = vecVariables.at( nIndex );

		if constexpr ( std::is_same_v<T , bool> )
			return entry.bValue;
		else if constexpr ( std::is_same_v<T , int> )
			return entry.iValue;
		else if constexpr ( std::is_same_v<T , float> )
			return entry.flValue;
		else if constexpr ( std::is_same_v<T , Color4> )
			return entry.colValue;
		else
		{
			static_assert( sizeof( T ) == 0 , "Unsupported config variable type" );
			return entry.bValue;
		}
	}

	template<typename T>
	auto CCookieConfig::Set( const std::size_t nIndex , const T& value ) -> bool
	{
		if ( nIndex >= vecVariables.size() )
			return false;

		auto& entry = vecVariables[nIndex];

		if constexpr ( std::is_same_v<T , bool> )
		{
			entry.eType = EConfigValueType::Bool;
			entry.bValue = value;
		}
		else if constexpr ( std::is_same_v<T , int> )
		{
			entry.eType = EConfigValueType::Int;
			entry.iValue = value;
		}
		else if constexpr ( std::is_same_v<T , float> )
		{
			entry.eType = EConfigValueType::Float;
			entry.flValue = value;
		}
		else if constexpr ( std::is_same_v<T , Color4> )
		{
			entry.eType = EConfigValueType::Color4;
			entry.colValue = value;
		}
		else
		{
			return false;
		}

		return true;
	}
}

#define COOKIE_CFG_ADD( TYPE , NAME , DEFAULT ) \
	inline const std::size_t NAME = CookieCore::CCookieConfig::AddVariable<TYPE>( \
		hash_64_fnv1a_const( #NAME ) , \
		hash_64_fnv1a_const( #TYPE ) , \
		DEFAULT )

#define COOKIE_CFG_GET( TYPE , NAME ) \
	CookieCore::CCookieConfig::Get<TYPE>( NAME )

#define COOKIE_CFG_SET( TYPE , NAME , VALUE ) \
	CookieCore::CCookieConfig::Set<TYPE>( NAME , VALUE )
