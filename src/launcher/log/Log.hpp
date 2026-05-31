#pragma once

#include <string_view>

namespace injlog
{
	enum class Level
	{
		Info,
		Warn,
		Error,
	};

	bool init();
	void shutdown();
	void flush();
	void installCrashHandler();

	void write(Level level, std::string_view message);

	inline void info(std::string_view message)
	{
		write(Level::Info, message);
	}

	inline void warn(std::string_view message)
	{
		write(Level::Warn, message);
	}

	inline void error(std::string_view message)
	{
		write(Level::Error, message);
	}
}
