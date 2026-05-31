#pragma once

#define IDR_COOKIE_DLL 101

struct CookieBootstrapParam_t
{
	uint64_t ArgsSize = 0;
	char DllPath[MAX_PATH]{};
};
