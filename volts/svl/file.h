#pragma once

#include "types.h"
#include "platform.h"

#include <spdlog/spdlog.h>

#include <vector>
#include <filesystem>

namespace svl
{
    namespace mode
    {
        constexpr u8 read = (1 << 0);
        constexpr u8 write = (1 << 1);
    }

    struct file_handle 
    {
        virtual ~file_handle() {}
        virtual void seek(u64 pos) {}
        virtual u64 tell() const { return 0; }
        virtual u64 size() const { return 0; }

        virtual void read(void* out, u64 num) {}
        virtual void write(const void* in, u64 num) {}
    };

    struct file
    {
        void seek(u64 pos)
        {
            handle->seek(pos);
        }

        u64 tell() const
        {
            return handle->tell();
        }

        template<typename T>
        T read()
        {
            T val;
            handle->read(&val, sizeof(T));
            return val;
        }

        template<typename T>
        std::vector<T> read(u64 num)
        {
            std::vector<T> vec(num);
            handle->read(vec.data(), num * sizeof(T));
            return vec;
        }
        
        void read(void* out, u64 num)
        {
            handle->read(out, num);
        }

        template<typename T>
        void write(const T& val)
        {
            handle->write(&val, sizeof(T));
        }

        template<typename T>
        void write(const std::vector<T>& vec)
        {
            handle->write(vec.data(), vec.size() * sizeof(T));
        }

        void write(const void* ptr, u64 num)
        {
            handle->write(ptr, num);
        }

        bool valid() const
        {
            return !!handle;
        }

        file()
            : handle(nullptr)
        {}

        file(file_handle* ptr)
            : handle(ptr)
        {}

        file(const file& f) : handle(f.handle) {}

    private:

        std::shared_ptr<file_handle> handle;
    };

    file open(const std::filesystem::path& path, u8 mo);
    
    file from(const std::vector<svl::byte>& vec);
}