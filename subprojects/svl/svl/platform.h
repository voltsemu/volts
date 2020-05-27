#pragma once

#if defined(__clang__)
#   define CL_CLANG 1
#elif defined(__GNUG__) || defined(__GNUC__) || defined(__GNU__)
#   define CL_GCC 1
#elif defined(_MSC_VER)
#   define CL_MSVC 1
#else
#   error "unsupported compiler"
#endif


#if defined(_WIN32) || defined(_WIN64)
#   define SYS_WINDOWS 1
#elif defined(__APPLE__) && defined(__MACH__)
#   define SYS_OSX 1
#elif defined(__linux__) || defined(__unix__)
#   define SYS_LINUX 1
#else
#   error "unsupported platform"
#endif

#if SYS_WINDOWS
#   include <Windows.h>
#endif


// alias alloca on windows (issue 3)
#if SYS_WINDOWS
#   define ALLOCA(N) _malloca(N)
#else
#   define ALLOCA(N) alloca(N)
#endif
