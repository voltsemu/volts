#pragma once

#define CL_CLANG (defined(__clang__))
#define CL_GCC (defined(__GNUG__) || defined(__GNUC__) || defined(__GNU__))
#define CL_MSVC (defined(_MSC_VER))

#if !SYS_CLANG && !CL_GCC && !CL_MSVC
#   error "unsupported compiler"
#endif

#define SYS_WINDOWS (defined(_WIN32) || defined(_WIN64))
#define SYS_OSX (defined(__APPLE__) && defined(__MACH__))
#define SYS_LINUX (defined(__linux__) || defined(__unix__))

#if !SYS_WINDOWS && !SYS_OSX && !SYS_LINUX
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
