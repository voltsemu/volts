#pragma once

#include "svl/types.h"

#include <vector>
#include <cstdio>
#include <filesystem>

namespace svl
{
    struct iostream
    {
        iostream() {}
        virtual void seek(std::size_t len) = 0;
        virtual std::size_t tell() const = 0;
        virtual std::size_t size() const = 0;
        virtual void read(void* ptr, std::size_t len) = 0;
        virtual void write(void* ptr, std::size_t len) = 0;

        virtual ~iostream() {}
    };

    struct memstream : iostream
    {
        memstream();
        memstream(const std::vector<svl::byte>& o);

        memstream(const memstream&) = default;

        virtual void seek(std::size_t len) override;
        virtual std::size_t tell() const override;
        virtual std::size_t size() const override;
        virtual void read(void* ptr, std::size_t len) override;
        virtual void write(void* ptr, std::size_t len) override;
        std::vector<svl::byte> take_vector();

        virtual ~memstream() override {}

    private:
        std::vector<svl::byte> vec;
        svl::u32 cursor = 0;
    };
    
    struct fstream : iostream
    {
        // support binary, text, in, out
        fstream(const std::filesystem::path& path, std::ios_base::openmode mode = std::ios::in);

        fstream(fstream&& o);

        virtual ~fstream() override;

        virtual void seek(std::size_t len) override;

        virtual std::size_t tell() const override;

        virtual std::size_t size() const override;

        virtual void read(void* ptr, std::size_t len) override;

        virtual void write(void* ptr, std::size_t len) override;
    private:
        std::FILE* file;
    };

    template<typename T>
    T read(iostream& stream)
    {
        T val;
        stream.read((void*)&val, sizeof(T));
        return val;
    }

    template<typename T = svl::byte>
    std::vector<T> read_n(iostream& stream, std::size_t num)
    {
        std::vector<T> val(num);
        stream.read((void*)val.data(), num * sizeof(T));
        return val;
    }

    template<typename T>
    void write(iostream& stream, T val)
    {
        stream.write((void*)&val, sizeof(T));
    }

    template<typename T = svl::byte>
    void write_n(iostream& stream, const std::vector<T>& val)
    {
        stream.write((void*)val.data(), sizeof(T) * val.size());
    }
    
    void write_utf8(const std::filesystem::path& path, const std::string& str);
}