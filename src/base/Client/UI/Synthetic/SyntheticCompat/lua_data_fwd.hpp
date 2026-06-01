#pragma once

#include <string>

struct lua_data
{
	std::string name;
	std::string date;
	bool loaded = false;
};

struct config_data
{
	std::string name;
	std::string date;
};
