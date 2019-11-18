#pragma once

#include <fstream>
#include <filesystem>
#include <vector>
#include <cstdint>

namespace svl::streams
{
    namespace fs = std::filesystem;

    template<typename T>
    T read(std::istream& stream)
    {
        T val;
        stream.read((char*)&val, sizeof(T));
        return val;
    }

    std::vector<std::uint8_t> read_n(std::istream& stream, std::streamsize n);

    void write_utf8(fs::path path, const std::string& str);
}