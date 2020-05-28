#pragma once

#include <unordered_map>
#include <string>

#include <svl/file.h>
#include <svl/types.h>

namespace volts::loader::tar
{
    using OffsetMap = std::unordered_map<std::string, svl::u64>;

    struct Object
    {
        OffsetMap offsets;
        svl::File source;
    };

    Object load(svl::File&& source);
}