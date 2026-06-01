#pragma once

#include <filesystem>
#include <thread>
#include <chrono>
#include <cwctype>
#include <vector>

#include <windows.h>
#include <tchar.h>

#include <d3d9.h>

#pragma comment(lib, "d3d9.lib")

#include "Version.hpp"
#include "payload.hpp"
#include "log/Log.hpp"
#include "log/LogFile.hpp"
#include "log/LogWindow.hpp"
#include "log/CrashHandler.hpp"
#include "injector/injector.hpp"
#include "memory/memory.hpp"
#include "vars/vars.hpp"
#include "utils/utils.hpp"
#include "gui/Menu.hpp"

using namespace std::chrono_literals;
