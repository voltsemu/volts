#pragma once

#include "svl/types.h"

#include <ostream>
#include <vector>
#include <optional>

namespace volts::loader::unself
{
    std::optional<std::ostream> load(std::istream& file, std::vector<svl::byte> key = {});
}