#pragma once

#include <string>
#include <vector>

struct lua_data;

namespace SyntheticLua
{
	auto Init() noexcept -> void;
	auto Shutdown() noexcept -> void;

	auto SyncScriptList() noexcept -> void;

	auto LoadScriptIntoEditor( const std::string& scriptName ) noexcept -> bool;
	auto SaveActiveScript( const std::string& scriptName , const std::string& source ) noexcept -> bool;
	auto DeleteScript( const std::string& scriptName ) noexcept -> bool;
	auto CreateScript( const std::string& scriptName ) noexcept -> bool;

	auto RunSource( const std::string& source ) noexcept -> bool;

	auto GetLastError() noexcept -> const std::string&;
}
