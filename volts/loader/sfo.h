#pragma once

#include "svl/types.h"
#include "svl/stream.h"

#include <cstdint>
#include <map>
#include <optional>

namespace volts::loader::sfo
{
    enum class format : svl::u16
    {
        string = 516,
        array = 4,
        integer = 1028,
    };

    struct value
    {
        format type;

        std::vector<svl::byte> data;
    };

    using object = std::map<std::string, value>;

    std::optional<object> load(svl::iostream& stream);
}