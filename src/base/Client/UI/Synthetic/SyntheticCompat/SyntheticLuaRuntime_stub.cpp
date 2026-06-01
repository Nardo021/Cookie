#include <Client/UI/Synthetic/SyntheticLuaRuntime.hpp>

#include <framework/settings/variables.h>

namespace SyntheticLua
{
	auto Init() noexcept -> void
	{
	}

	auto Shutdown() noexcept -> void
	{
	}

	auto SyncScriptList() noexcept -> void
	{
		var->c_lua.data.clear();
	}

	auto LoadScriptIntoEditor( const std::string& /*scriptName*/ ) noexcept -> bool
	{
		return false;
	}

	auto SaveActiveScript( const std::string& /*scriptName*/ , const std::string& /*source*/ ) noexcept -> bool
	{
		return false;
	}

	auto DeleteScript( const std::string& /*scriptName*/ ) noexcept -> bool
	{
		return false;
	}

	auto CreateScript( const std::string& /*scriptName*/ ) noexcept -> bool
	{
		return false;
	}

	auto RunSource( const std::string& /*source*/ ) noexcept -> bool
	{
		return false;
	}

	auto GetLastError() noexcept -> const std::string&
	{
		static const std::string kMsg = "Lua is not available in this build (install vcpkg lua dependency).";
		return kMsg;
	}
}
