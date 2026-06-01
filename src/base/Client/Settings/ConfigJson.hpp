#pragma once

#include <algorithm>
#include <cstdint>
#include <string>

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/ostreamwrapper.h>

namespace ConfigJson
{
	inline auto WriteColor(
		rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer ,
		const char* name ,
		const float color[4] ) -> void
	{
		writer.String( name );
		writer.StartArray();
		writer.Double( color[0] );
		writer.Double( color[1] );
		writer.Double( color[2] );
		writer.Double( color[3] );
		writer.EndArray();
	}

	inline auto ReadColor( const rapidjson::Value& obj , const char* name , float color[4] ) -> void
	{
		if ( !obj.HasMember( name ) || !obj[name].IsArray() || obj[name].Size() != 4 )
			return;

		for ( rapidjson::SizeType i = 0; i < 4; ++i )
			color[i] = static_cast<float>( obj[name][i].GetDouble() );
	}

	inline auto ReadBool( const rapidjson::Value& obj , const char* name , bool& out ) -> void
	{
		if ( obj.HasMember( name ) && obj[name].IsBool() )
			out = obj[name].GetBool();
	}

	inline auto ReadInt( const rapidjson::Value& obj , const char* name , int& out ) -> void
	{
		if ( obj.HasMember( name ) && obj[name].IsInt() )
			out = obj[name].GetInt();
	}

	inline auto ReadUInt( const rapidjson::Value& obj , const char* name , unsigned int& out ) -> void
	{
		if ( obj.HasMember( name ) && obj[name].IsUint() )
			out = obj[name].GetUint();
	}

	inline auto ReadFloat( const rapidjson::Value& obj , const char* name , float& out ) -> void
	{
		if ( obj.HasMember( name ) && obj[name].IsNumber() )
			out = static_cast<float>( obj[name].GetDouble() );
	}

	inline auto ReadString( const rapidjson::Value& obj , const char* name , char* out , std::size_t outSize ) -> void
	{
		if ( !out || outSize == 0 )
			return;

		if ( !obj.HasMember( name ) || !obj[name].IsString() )
			return;

		const char* src = obj[name].GetString();
		std::size_t i = 0;
		for ( ; i + 1 < outSize && src[i] != '\0'; ++i )
			out[i] = src[i];
		out[i] = '\0';
	}

	inline auto WriteString(
		rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer ,
		const char* name ,
		const char* value ) -> void
	{
		writer.String( name );
		writer.String( value ? value : "" );
	}

	inline auto ClampWeaponClassIndex( int index ) -> int
	{
		return std::clamp( index , 0 , 6 );
	}
}
