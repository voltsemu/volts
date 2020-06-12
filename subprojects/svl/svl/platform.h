#pragma once

#if defined(_WIN32)
#   define SYS_WINDOWS 1
#   define _UNICODE 1
#   define WIN32_LEAN_AND_MEAN 1
#   include <Windows.h>
#   include <malloc.h>
#   define ALLOCA(N) _alloca(N)
#elif defined(__APPLE__)
#   define SYS_OSX 1
#   define ALLOCA(N) alloca(N)
#elif defined(__linux__)
#   define SYS_LINUX 1
#   define ALLOCA(N) alloca(N)
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


#if defined(__clang__)
#   define CL_CLANG 1
#elif defined(__GNUG__) || defined(__GNUC__) || defined(__GNU__)
#   define CL_GCC 1
#elif defined(_MSC_VER)
#   define CL_MSVC 1
#else
#   error "unsupported compiler"
#endif

#ifndef CL_CLANG
#   define CL_CLANG 0
#endif

#ifndef CL_GCC
#   define CL_GCC 0
#endif

#ifndef CL_MSVC
#   define CL_MSVC 0
#endif

#if CL_MSVC
#   include <intrin.h>
#   define CPUID(leaf, regs) __cpuid((int*)regs, leaf)
#else
#   include <cpuid.h>
#   define CPUID(leaf, regs) __cpuid(leaf, regs[0], regs[1], regs[2], regs[3])
#endif