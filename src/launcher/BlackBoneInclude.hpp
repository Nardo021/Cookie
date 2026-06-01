#pragma once

// Launcher only links BlackBone.lib; no .NET metadata / ATL in headers (avoids blackbone::std vs ::std).
#ifndef BLACKBONE_NO_NET
#define BLACKBONE_NO_NET
#endif

#include <BlackBone/Process/Process.h>
