#pragma once

#include "svl/types.h"
#include "svl/stream.h"

#include <vector>
#include <optional>

namespace volts::loader::unself
{
    std::vector<svl::memstream> load_sce(svl::iostream& file);
    std::vector<svl::byte> load_self(svl::iostream& file, std::vector<svl::byte> key = {});
}