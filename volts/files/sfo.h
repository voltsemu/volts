#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include <istream>
#include <optional>

namespace volts::files::sfo
{
    enum class format : std::uint16_t
    {
        string = 516,
        array = 4,
        integer = 1028,
    };

    struct value
    {
        format type;

        std::vector<std::uint8_t> data;
    };

    std::optional<std::map<std::string, value>> load(std::istream& stream);
}