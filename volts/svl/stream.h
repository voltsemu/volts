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

    void write_utf8(fs::path path, const std::string& str);

    template<typename T>
    class vectorbuf : public std::streambuf
    {
        std::vector<T>* buffer;
    protected:
        
        virtual int_type overflow(int_type c)
        {
            buffer->push_back(static_cast<T>(c));
            return c;
        }

    public:
        vectorbuf(std::vector<T>* ptr)
            : buffer(ptr)
        {}
    };
}