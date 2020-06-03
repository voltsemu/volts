#pragma once

#if defined(_WIN32)
#   define SYS_WINDOWS 1
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
