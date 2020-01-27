#pragma once

#include "svl/types.h"
#include "svl/file.h"

#include <vector>
#include <optional>

namespace volts::loader::unself
{
    std::vector<svl::file> load_sce(svl::file file);
    svl::file load_self(svl::file file, std::vector<svl::byte> key = {});
}