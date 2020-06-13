#pragma once

#include "svl/platform.h"

#define STR_INNER(X) #X
#define STR(X) STR_INNER(X)

#define HERE __FILE__ ":" STR(__LINE__)

// TODO: compiler toggle for release builds

// a macro to check builtin error codes for each platform
#if SYS_WINDOWS
#   define CHECK(expr) if (auto err = (expr); !err) { svl::fatal(HERE "{} Win32 error {}", err, ::GetLastError()); }
#else
#   include <errno.h>
#   define CHECK(expr) if (auto err = (expr); err == -1) { svl::fatal(HERE " Posix error {}", strerror(errno)); }
#endif

// an assert impl
#define ENSURE(expr) if (auto err = (expr); !err) { svl::fatal(HERE "expr" #expr "failed"); }