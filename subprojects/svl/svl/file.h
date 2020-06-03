#pragma once

#if __has_include(<filesystem>)
#   include <filesystem>
namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
#   include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#   error "<filesystem> header missing"
#endif

#include <memory>

#include "svl/types.h"

namespace svl
{
    struct FileHandle
    {
        virtual ~FileHandle() { }

        virtual void read(void* out, usize len) = 0;
        virtual void write(const void* in, usize len) = 0;

        virtual usize tell() const = 0;
        virtual usize size() const = 0;
        virtual void seek(usize pos) = 0;
    };

    struct File
    {
        usize tell() const { return handle->tell(); }
        usize size() const { return handle->size(); }

        File(FileHandle* ptr)
            : handle(ptr)
        { }
    private:
        std::unique_ptr<FileHandle> handle;
    };

    File open(const fs::path& path);
}