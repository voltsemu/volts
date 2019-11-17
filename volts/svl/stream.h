#pragma once

#include <fstream>
#include <vector>
#include <cstdint>

namespace svl::streams
{
    template<typename T>
    T read(std::fstream& stream)
    {
        T val;
        stream.read((char*)&val, sizeof(T));
        return val;
    }

    std::vector<std::uint8_t> read_n(std::fstream& stream, std::streamsize n)
    {
        std::vector<std::uint8_t> val(n);
        stream.read((char*)val.data(), n);
        return std::move(val);
    }
}