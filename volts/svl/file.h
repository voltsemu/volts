#pragma once

#include "types.h"
#include "platform.h"

#include <spdlog/spdlog.h>

#if SYS_WINDOWS
#   include "windows.h"
#endif

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
    
#if SYS_WINDOWS
    // win32 file handle wrapper
    struct win32_file : file_handle
    {
        virtual ~win32_file() override
        {
            FindClose(handle);
        }

        win32_file(const HANDLE&& h)
            : handle(h)
        {}

        virtual void seek(u64 pos) override 
        {
            LARGE_INTEGER p;
            p.QuadPart = pos;

            SetFilePointerEx(handle, p, nullptr, FILE_BEGIN);
        }

        virtual u64 tell() const override 
        {
            LARGE_INTEGER pos;
            pos.QuadPart = 0;
            SetFilePointerEx(handle, pos, &pos, FILE_CURRENT);

            return pos.QuadPart;
        }
        
        virtual u64 size() const override
        {
            LARGE_INTEGER len;
            GetFileSizeEx(handle, &len);
            return len.QuadPart;
        }

        virtual void read(void* out, u64 num) override 
        {
            DWORD n;
            if(!ReadFile(handle, out, num, &n, nullptr))
            {
                spdlog::error("reading from file failed {}", GetLastError());
            }
        }

        virtual void write(const void* in, u64 num) override 
        {
            WriteFile(handle, in, num, nullptr, nullptr);
        }

    private:
        const HANDLE handle;
    };
#else
    // posix file handle wrapper
    struct posix_file : file_handle
    {
        virtual ~posix_file() override
        {
            std::fclose(handle);
        }

        posix_file(std::FILE* h)
            : handle(h)
        {}

    private:
        std::FILE* handle;
    };
#endif

    // in memory file handle wrapper
    struct ram_file : file_handle
    {
        ram_file(const std::vector<byte>& vec)
            : handle(vec)
        {}

        virtual void seek(u64 pos) override 
        {
            cursor = pos;
            if(handle.max_size() + cursor < pos)
                handle.resize(pos);
        }

        virtual u64 size() const override
        {
            return handle.size();
        }

        virtual u64 tell() const override 
        { 
            return cursor; 
        }

        virtual void read(void* out, u64 num) override 
        {
            if(cursor + num > handle.max_size())
                handle.resize(cursor + num);

            std::memcpy(out, handle.data() + cursor, num);
            cursor += num;
        }

        virtual void write(const void* in, u64 num) override 
        {
            if(num + cursor > handle.max_size())
                handle.resize(cursor + num);

            std::memcpy(handle.data() + cursor, in, num);
            cursor += num;
        }

    private:
        u64 cursor = 0;
        std::vector<byte> handle = {};
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
    
    file from(std::vector<svl::byte> vec);
}