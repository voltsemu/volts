#pragma once

#if defined(_WIN32)
#   define SYS_WINDOWS 1
#   define _UNICODE 1
#   define WIN32_LEAN_AND_MEAN 1
#   include <Windows.h>
#elif defined(__APPLE__)
#   define SYS_OSX 1
#elif defined(__linux__)
#   define SYS_LINUX 1
#else
#   error "unsupported operating system"
#endif

#ifndef SYS_WINDOWS
#   define SYS_WINDOWS 0
#endif

#ifndef SYS_OSX
#   define SYS_OSX 0
#endif

#ifndef SYS_LINUX
#   define SYS_LINUX 0
#endif
