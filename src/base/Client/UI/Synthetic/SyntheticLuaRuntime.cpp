#include "SyntheticLuaRuntime.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

#include <Client/Features/Combat/Legit/Aimbot.hpp>
#include <Client/Features/Combat/Rage/Ragebot.hpp>
#include <Client/Features/Visuals/Esp.hpp>
#include <Client/UI/Synthetic/SyntheticNotifyBridge.hpp>
#include <DllLauncher.hpp>
#include <framework/settings/functions.h>
#include <framework/settings/variables.h>

extern "C"
{
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}

namespace SyntheticLua
{
	namespace
	{
		lua_State* s_lua = nullptr;
		std::string s_lastError;
		std::string s_activeScript;

		auto EnsureScriptsDir() noexcept -> std::string
		{
			const std::string dir = GetDllDir() + "lua\\";
			std::error_code ec;
			std::filesystem::create_directories( dir , ec );
			return dir;
		}

		auto SanitizeName( std::string name ) -> std::string
		{
			for ( char& c : name )
			{
				if ( c == '\\' || c == '/' || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|' )
					c = '_';
			}
			if ( name.size() < 5 || name.substr( name.size() - 4 ) != ".lua" )
				name += ".lua";
			return name;
		}

		auto ScriptPath( const std::string& name ) noexcept -> std::string
		{
			return EnsureScriptsDir() + SanitizeName( name );
		}

		auto CookiePrint( lua_State* L ) -> int
		{
			std::string line;
			const int argc = lua_gettop( L );
			for ( int i = 1; i <= argc; ++i )
			{
				if ( i > 1 )
					line += '\t';
				line += luaL_tolstring( L , i , nullptr );
				lua_pop( L , 1 );
			}
			if ( !line.empty() )
				SyntheticNotifyBridge::Push( N_TYPE_INFO , line.c_str() );
			return 0;
		}

		auto CookieSetBool( lua_State* L , bool* target ) -> int
		{
			*target = lua_toboolean( L , 1 ) != 0;
			return 0;
		}

		auto CookieAimbotEnabled( lua_State* L ) -> int
		{
			return CookieSetBool( L , &Aimbot::config.enabled );
		}

		auto CookieRageEnabled( lua_State* L ) -> int
		{
			return CookieSetBool( L , &Ragebot::config.enabled );
		}

		auto CookieEspEnabled( lua_State* L ) -> int
		{
			return CookieSetBool( L , &ESP::config.enabled );
		}

		auto RegisterApi( lua_State* L ) -> void
		{
			lua_newtable( L );

			lua_pushcfunction( L , CookiePrint );
			lua_setfield( L , -2 , "print" );

			lua_pushcfunction( L , CookieAimbotEnabled );
			lua_setfield( L , -2 , "set_aimbot" );

			lua_pushcfunction( L , CookieRageEnabled );
			lua_setfield( L , -2 , "set_rage" );

			lua_pushcfunction( L , CookieEspEnabled );
			lua_setfield( L , -2 , "set_esp" );

			lua_setglobal( L , "cookie" );
		}
	}

	auto Init() noexcept -> void
	{
		if ( s_lua )
			return;

		s_lua = luaL_newstate();
		luaL_openlibs( s_lua );
		RegisterApi( s_lua );
	}

	auto Shutdown() noexcept -> void
	{
		if ( s_lua )
		{
			lua_close( s_lua );
			s_lua = nullptr;
		}
	}

	auto SyncScriptList() noexcept -> void
	{
		var->c_lua.data.clear();

		const std::string dir = EnsureScriptsDir();
		std::error_code ec;
		if ( !std::filesystem::exists( dir , ec ) )
			return;

		for ( const auto& entry : std::filesystem::directory_iterator( dir , ec ) )
		{
			if ( !entry.is_regular_file() )
				continue;
			if ( entry.path().extension() != ".lua" )
				continue;

			lua_data item{};
			item.name = entry.path().filename().string();
			item.date = "on disk";
			item.loaded = false;
			var->c_lua.data.push_back( std::move( item ) );
		}
	}

	auto LoadScriptIntoEditor( const std::string& scriptName ) noexcept -> bool
	{
		std::ifstream file( ScriptPath( scriptName ) );
		if ( !file )
		{
			s_lastError = "Failed to open script";
			return false;
		}

		std::stringstream buffer;
		buffer << file.rdbuf();
		text_editor->set_text( buffer.str() );
		s_activeScript = SanitizeName( scriptName );
		var->c_lua.editable = s_activeScript;
		return true;
	}

	auto SaveActiveScript( const std::string& scriptName , const std::string& source ) noexcept -> bool
	{
		const std::string path = ScriptPath( scriptName );
		std::ofstream file( path , std::ios::trunc );
		if ( !file )
		{
			s_lastError = "Failed to save script";
			return false;
		}

		file << source;
		s_activeScript = SanitizeName( scriptName );
		SyncScriptList();
		return true;
	}

	auto DeleteScript( const std::string& scriptName ) noexcept -> bool
	{
		std::error_code ec;
		const bool ok = std::filesystem::remove( ScriptPath( scriptName ) , ec );
		if ( !ok )
			s_lastError = "Failed to delete script";
		else
			SyncScriptList();
		return ok;
	}

	auto CreateScript( const std::string& scriptName ) noexcept -> bool
	{
		const std::string path = ScriptPath( scriptName );
		if ( std::filesystem::exists( path ) )
		{
			s_lastError = "Script already exists";
			return false;
		}

		std::ofstream file( path );
		if ( !file )
		{
			s_lastError = "Failed to create script";
			return false;
		}

		SyncScriptList();
		return LoadScriptIntoEditor( scriptName );
	}

	auto RunSource( const std::string& source ) noexcept -> bool
	{
		if ( !s_lua )
			Init();

		if ( luaL_loadbuffer( s_lua , source.c_str() , source.size() , "cookie_lua" ) != LUA_OK )
		{
			s_lastError = lua_tostring( s_lua , -1 );
			lua_pop( s_lua , 1 );
			return false;
		}

		if ( lua_pcall( s_lua , 0 , 0 , 0 ) != LUA_OK )
		{
			s_lastError = lua_tostring( s_lua , -1 );
			lua_pop( s_lua , 1 );
			return false;
		}

		s_lastError.clear();
		return true;
	}

	auto GetLastError() noexcept -> const std::string&
	{
		return s_lastError;
	}
}
