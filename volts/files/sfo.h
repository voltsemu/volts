#pragma once

#include "svl/types.h"

#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include <istream>
#include <optional>

namespace volts::files::sfo
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

    std::optional<std::map<std::string, value>> load(std::istream& stream);
}