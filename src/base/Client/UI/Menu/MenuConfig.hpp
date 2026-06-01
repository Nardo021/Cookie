#pragma once

#include <Client/Settings/ConfigSchema.hpp>

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/ostreamwrapper.h>

namespace MenuConfig
{
	inline constexpr int kSchemaVersion = ConfigSchema::kVersion;

	auto WriteSettings( rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer ) -> void;
	auto ReadSettings( const rapidjson::Document& document ) -> void;
}
