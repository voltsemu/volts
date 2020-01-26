#pragma once

#include "types.h"
#include "platform.h"

#if SYS_WINDOWS
#   include "windows.h"
#endif

#include <vector>
#include <filesystem>

namespace svl
{
    using mode = u8;
    constexpr mode read = (1 << 0);
    constexpr mode write = (1 << 1);

    struct file_handle 
    {
        virtual ~file_handle() {}
        virtual void seek(u64 pos) = 0;
        virtual u64 tell() const = 0;
        virtual u64 size() const = 0;

        virtual void read(void* out, u64 num) = 0;
        virtual void write(void* in, u64 num) = 0;
    };
    
#if SYS_WINDOWS
    // win32 file handle wrapper
    struct win32_file : file_handle
    {
        virtual ~win32_file() override
        {
            CloseHandle(handle);
        }

        win32_file(HANDLE h)
            : handle(h)
        {}

        virtual void seek(u64 pos) override 
        {

        }

        virtual u64 tell() const override 
        {
            
        }
        
        virtual u64 size() const override
        {
            LARGE_INTEGER len;
            GetFileSizeEx(handle, &len);
            return len.QuadPart;
        }

        virtual void read(void* out, u64 num) override 
        {

        }

        virtual void write(void* in, u64 num) override 
        {

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
        ram_file(std::vector<byte>&& vec)
            : handle(vec)
        {}

        virtual void seek(u64 pos) override {}
        virtual u64 tell() const override { return cursor; }

        virtual void read(void* out, u64 num) override {}
        virtual void write(void* in, u64 num) override {}

    private:
        u64 cursor = 0;
        std::vector<byte> handle;
    };

    struct file
    {
        friend file open(const std::filesystem::path& path, mode m);
        
        template<typename T>
        friend file from(std::vector<T>&& vec);

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

    private:
        file(file_handle* ptr)
            : handle(ptr)
        {}

        std::shared_ptr<file_handle> handle = nullptr;
    };

    file open(const std::filesystem::path& path, mode m)
    {
#if SYS_WINDOWS
        DWORD access = 0;
        if(m & write)
            access |= GENERIC_WRITE;

        if(m & read)
            access |= GENERIC_READ;

        HANDLE f = CreateFileW(path.c_str(), access, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        return { new win32_file(f) };
#else
        std::string access;

        if(m & write)
            access += "w";

        if(m & read)
            access += "r";

        std::FILE* f = std::fopen(path.c_str(), access);
        return { new posix_file(f) };
#endif
    }
    
    template<typename T>
    file from(std::vector<T>&& vec)
    {
        return { new ram_file(vec) };
    }

    template<typename T>
    std::vector<T> read_n(file& f, u64 num)
    {
        std::vector<T> vec(num);
        f.read((void*)vec.data(), num * sizeof(T));
        return vec;
    }

    template<typename T>
    void write_n(file& f, const std::vector<T>& vec)
    {
        f.write((void*)vec.data(), vec.size() * sizeof(T));
    }
}