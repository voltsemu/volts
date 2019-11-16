#pragma once

#include <filesystem>
#include <algorithm>
#include <fstream>
#include <string>
#include <optional>
#include <map>
#include <vector>
#include <future>
#include <execution>

#define CXXOPTS_NO_EXCEPTIONS
#define CXXOPTS_NO_RTTI

#include "cxxopts/cxxopts.h"

#define FMT_EXCEPTIONS 0
#include <fmt/format.h>

#include <Cthulhu/Meta/Macros.h>
#include <Cthulhu/Types/Types.h>

#include <Cthulhu/Collections/String.h>

#include <Cthulhu/Interfaces/Stream.h>
#include <Cthulhu/System/Binary.h>
#include <Cthulhu/IO/File.h>

#if OS_WINDOWS
#   include <Windows.h>
#endif

namespace Volts
{
    using namespace Cthulhu;
    using namespace Types;
}