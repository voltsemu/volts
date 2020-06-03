#pragma once

#include "svl/platform.h"

#include <memory>
#include <fmt/core.h>

#if SYS_WINDOWS
#   define CHECK_ERRNO(expr) if (int err = (expr); err != -1) { svl::log::fatal(); }
#else
#   define CHECK_RESULT(expr) if (auto err = (expr); !err) { svl::log::fatal(); }
#endif

namespace svl
{

}