#pragma once

#include <svl/platform.h>

#if SYS_WINDOWS
#   include "windows/app.h"
#   include "windows/nav.h"
#   include "windows/item.h"
#   include "windows/simple.h"
#define GAC_MAIN(...) int WINAPI wWinMain(HINSTANCE inst, HINSTANCE prev, PWSTR cmd, int show) { __VA_ARGS__; }
#elif SYS_OSX
#   include "darwin/app.h"
#elif SYS_UNIX
#   include "linux/app.h"
#endif