#include "CCookieConfig.hpp"

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <ShlObj.h>

namespace CookieCore
{
	auto CCookieConfig::TypeToEnum( const std::uint64_t uTypeHash ) -> EConfigValueType
	{
		if ( uTypeHash == hash_64_fnv1a_const( "bool" ) )
			return EConfigValueType::Bool;
		if ( uTypeHash == hash_64_fnv1a_const( "int" ) )
			return EConfigValueType::Int;
		if ( uTypeHash == hash_64_fnv1a_const( "float" ) )
			return EConfigValueType::Float;
		if ( uTypeHash == hash_64_fnv1a_const( "Color4" ) )
			return EConfigValueType::Color4;

		return EConfigValueType::Bool;
	}

	auto CCookieConfig::EnumToTypeHash( EConfigValueType eType ) -> std::uint64_t
	{
		switch ( eType )
		{
		case EConfigValueType::Bool: return hash_64_fnv1a_const( "bool" );
		case EConfigValueType::Int: return hash_64_fnv1a_const( "int" );
		case EConfigValueType::Float: return hash_64_fnv1a_const( "float" );
		case EConfigValueType::Color4: return hash_64_fnv1a_const( "Color4" );
		default: return 0;
		}
	}

	auto CCookieConfig::GetVariableIndex( const std::uint64_t uNameHash ) -> std::size_t
	{
		for ( std::size_t i = 0; i < vecVariables.size(); ++i )
		{
			if ( vecVariables[i].uNameHash == uNameHash )
				return i;
		}

		return INVALID_INDEX;
	}

	auto CCookieConfig::GetConfigDirectory() -> std::string
	{
		wchar_t documentsPath[MAX_PATH]{};
		if ( FAILED( SHGetFolderPathW( nullptr , CSIDL_PERSONAL , nullptr , SHGFP_TYPE_CURRENT , documentsPath ) ) )
			return {};

		std::filesystem::path configDir = std::filesystem::path( documentsPath ) / XorStr( ".cookie" );
		std::error_code ec;
		std::filesystem::create_directories( configDir , ec );

		return configDir.string() + "\\";
	}

	auto CCookieConfig::Save( const std::string& fileName ) -> bool
	{
		const auto configDir = GetConfigDirectory();
		if ( configDir.empty() )
			return false;

		const auto filePath = configDir + fileName;
		std::ofstream configFile( filePath , std::ios::trunc );
		if ( !configFile.is_open() )
			return false;

		rapidjson::OStreamWrapper streamWrapper( configFile );
		rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer( streamWrapper );

		writer.SetIndent( '\t' , 1 );
		writer.SetMaxDecimalPlaces( 4 );
		writer.StartObject();
		{
			writer.String( XorStr( "CookieConfig" ) );
			writer.StartObject();
			{
				for ( const auto& entry : vecVariables )
				{
					const auto key = std::to_string( entry.uNameHash );
					writer.Key( key.c_str() );
					writer.StartObject();
					{
						writer.String( XorStr( "type" ) );
						writer.Uint64( entry.uTypeHash );

						writer.String( XorStr( "value" ) );
						switch ( entry.eType )
						{
						case EConfigValueType::Bool:
							writer.Bool( entry.bValue );
							break;
						case EConfigValueType::Int:
							writer.Int( entry.iValue );
							break;
						case EConfigValueType::Float:
							writer.Double( static_cast<double>( entry.flValue ) );
							break;
						case EConfigValueType::Color4:
							writer.StartArray();
							writer.Double( entry.colValue.r );
							writer.Double( entry.colValue.g );
							writer.Double( entry.colValue.b );
							writer.Double( entry.colValue.a );
							writer.EndArray();
							break;
						}
					}
					writer.EndObject();
				}
			}
			writer.EndObject();
		}
		writer.EndObject();

		return true;
	}

	auto CCookieConfig::Load( const std::string& fileName ) -> bool
	{
		const auto configDir = GetConfigDirectory();
		if ( configDir.empty() )
			return false;

		const auto filePath = configDir + fileName;
		std::ifstream configFile( filePath );
		if ( !configFile.is_open() )
			return false;

		rapidjson::IStreamWrapper streamWrapper( configFile );
		rapidjson::Document document;
		document.ParseStream( streamWrapper );

		if ( document.HasParseError() )
		{
			DEV_LOG(
				XorStr( "[error] CCookieConfig::Load: %s -> %s , %i\n" ) ,
				filePath.c_str() ,
				rapidjson::GetParseError_En( document.GetParseError() ) ,
				static_cast<int>( document.GetErrorOffset() ) );
			return false;
		}

		if ( !document.HasMember( XorStr( "CookieConfig" ) ) || !document[XorStr( "CookieConfig" )].IsObject() )
			return false;

		const auto& root = document[XorStr( "CookieConfig" )];

		for ( auto it = root.MemberBegin(); it != root.MemberEnd(); ++it )
		{
			if ( !it->value.IsObject() )
				continue;

			const auto uNameHash = std::strtoull( it->name.GetString() , nullptr , 10 );
			const auto nIndex = GetVariableIndex( uNameHash );
			if ( nIndex == INVALID_INDEX )
				continue;

			auto& entry = vecVariables[nIndex];
			const auto& object = it->value;

			if ( object.HasMember( XorStr( "type" ) ) && object[XorStr( "type" )].IsUint64() )
				entry.uTypeHash = object[XorStr( "type" )].GetUint64();

			entry.eType = TypeToEnum( entry.uTypeHash );

			if ( !object.HasMember( XorStr( "value" ) ) )
				continue;

			const auto& value = object[XorStr( "value" )];
			switch ( entry.eType )
			{
			case EConfigValueType::Bool:
				if ( value.IsBool() )
					entry.bValue = value.GetBool();
				break;
			case EConfigValueType::Int:
				if ( value.IsInt() )
					entry.iValue = value.GetInt();
				break;
			case EConfigValueType::Float:
				if ( value.IsNumber() )
					entry.flValue = static_cast<float>( value.GetDouble() );
				break;
			case EConfigValueType::Color4:
				if ( value.IsArray() && value.Size() == 4 )
				{
					entry.colValue.r = std::clamp( static_cast<float>( value[0].GetDouble() ) , 0.f , 1.f );
					entry.colValue.g = std::clamp( static_cast<float>( value[1].GetDouble() ) , 0.f , 1.f );
					entry.colValue.b = std::clamp( static_cast<float>( value[2].GetDouble() ) , 0.f , 1.f );
					entry.colValue.a = std::clamp( static_cast<float>( value[3].GetDouble() ) , 0.f , 1.f );
				}
				break;
			}
		}

		return true;
	}
}
