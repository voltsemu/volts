#pragma once

/**
 * @brief platform defines
 * 
 * platform specific defines
 * 
 * @def SYS_WINDOWS
 * 1 when building on windows else 0
 * 
 * @def SYS_OSX
 * 1 when compiling on mac osx else 0
 * 
 * @def SYS_UNIX
 * 1 when compiling on linux else 0
 * 
 * @def CL_CLANG
 * 1 when compiling using clang else 0
 * 
 * @def CL_GNU
 * 1 when compiling using g++ else 0
 * 
 * @def CL_MSVC
 * 1 when compiling using msvc else 0
 */

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
#   define CL_GNU 0
#endif

#ifndef CL_MSVC
#   define CL_MSVC 0
#endif

#if CL_CLANG || CL_GNU
#   define PACKED(name, ...) struct __attribute__ ((packed)) name __VA_ARGS__;
#elif CL_MSVC
#   define PACKED(name, ...) __pragma(pack(push, r1, 1)) struct name __VA_ARGS__; __pragma(pack(pop, r1, 2));
#endif