#pragma once

#include <cstdlib>

#if defined(_WIN32) || defined(WIN32)
#   define SYS_WIN 1
#   include <windows.h>
#   undef min
#elif defined(__MACH__) || defined(__APPLE__)
#   define SYS_OSX 1
#elif defined(__linux__) || defined(__unix__)
#   define SYS_UNIX 1
#else
#   error unrecognised platform
#endif

#ifndef SYS_WIN
#   define SYS_WIN 0
#endif

#ifndef SYS_OSX
#   define SYS_OSX 0
#endif

#ifndef SYS_UNIX
#   define SYS_UNIX 0
#endif
