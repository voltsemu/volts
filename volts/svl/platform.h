#pragma once

#if defined(_WIN32) || defined(_WIN64)
#   define SYS_WINDOWS 1
#elif defined(__APPLE__) && defined(__MACH__)
#   define SYS_OSX 1
#elif defined(__linux__) || defined(__unix__)
#   define SYS_UNIX 1
#else
#   error Unrecongnised platform
#endif

#ifndef SYS_WINDOWS
#   define SYS_WINDOWS 0
#endif

#ifndef SYS_OSX
#   define SYS_OSX 0
#endif

#ifndef SYS_UNIX
#   define SYS_UNIX 0
#endif