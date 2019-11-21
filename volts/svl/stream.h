#pragma once

#include <fstream>
#include <ostream>
#include <filesystem>
#include <vector>
#include <cstdint>
#include "types.h"

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

    template<typename T>
    std::vector<T> read_n(std::istream& stream, int n)
    {
        std::vector<T> ret(n);
        stream.read((char*)ret.data(), n * sizeof(T));
        return ret;
    }

    std::vector<u8> read_n(std::istream& stream, std::streamsize n);

    void write_utf8(const fs::path& path, const std::string& str);

    template<typename T>
    struct vectorbuf : public std::streambuf
    {
        vectorbuf(std::vector<T>& ptr)
        {
            setg((char*)ptr.data(), (char*)ptr.data(), (char*)(ptr.data() + ptr.size()));
        }
    };

    template<typename T>
    void write(std::ostream& stream, T val)
    {
        stream.write((char*)&val, sizeof(T));
    }
}
