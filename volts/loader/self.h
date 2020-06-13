#pragma once

#include <svl/file.h>

namespace volts::self
{
    svl::File load(svl::File &&source, const std::vector<uint8_t>& key = {});
}