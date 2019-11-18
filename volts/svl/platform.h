#pragma once

#if defined(_WIN32) || defined(_WIN64)
#   define SYS_WINDOWS 1
#elif defined(__APPLE__) && defined(__MACH__)
#   define SYS_OSX 1
#elif defined(__linux__) || defined(__unix__)
#   define SYS_UNIX 1
#else
#   error Unsupported platform
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

#if defined(__clang__)
#   define CL_CLANG 1
#elif defined(__GNUG__) || defined(__GNUC__) || defined(__GNU__)
#   define CL_GNU 1
#elif defined(_MSC_VER)
#   define CL_MSVC 1
#else
#   error Unsupported compiler
#endif

#ifndef CL_CLANG
#   define CL_CLANG 0
#endif

#ifndef CL_GNU
#   define CL_GNU 1
#endif

#ifndef CL_MSVC
#   define CL_MSVC 0
#endif