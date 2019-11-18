#pragma once

#include "svl/types.h"

#include <ostream>
#include <vector>

namespace volts::files::unself
{
    std::ostream load(std::istream& file, std::vector<svl::byte> key = {});
}