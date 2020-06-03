#pragma once

#include "svl/platform.h"

#define STR_INNER(X) #X
#define STR(X) STR_INNER(X)

#define HERE (STR(__FILE__) ":" STR(__LINE__))

#if SYS_WINDOWS
#   define CHECK_ERRNO(expr) if (int err = (expr); err != -1) { svl::log::fatal(HERE); }
#else
#   define CHECK_RESULT(expr) if (auto err = (expr); !err) { svl::log::fatal(HERE); }
#endif