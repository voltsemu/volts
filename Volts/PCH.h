#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <optional>
#include <map>
#include <vector>
#include <future>

#define CXXOPTS_NO_EXCEPTIONS
#define CXXOPTS_NO_RTTI

#include "cxxopts/cxxopts.h"

#define FMT_EXCEPTIONS 0
#include <fmt/format.h>

#include <Cthulhu/Meta/Macros.h>
#include <Cthulhu/Types/Types.h>

#include <Cthulhu/System/Binary.h>
#include <Cthulhu/Interfaces/Stream.h>
