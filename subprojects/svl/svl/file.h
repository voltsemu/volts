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
#include <vector>

#include "svl/types.h"

#include <fmt/core.h>

template<>
struct fmt::formatter<fs::path>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<typename T>
    auto format(const fs::path& path, T& ctx)
    {
        return format_to(ctx.out(), "{}", path.string());
    }
};

namespace svl
{
    struct FileHandle
    {
        virtual ~FileHandle() { }

        virtual void read(void *out, usize len) = 0;
        virtual void write(const void *in, usize len) = 0;

        virtual usize tell() const = 0;
        virtual usize size() const = 0;
        virtual void seek(usize pos) = 0;
    };

    struct File
    {
        template<typename T = svl::byte>
        std::vector<T> read(usize num)
        {
            std::vector<T> out(num);
            handle->read(out.data(), num * sizeof(T));
            return out;
        }

        template<typename T>
        T read()
        {
            T out;
            handle->read(&out, sizeof(T));
            return out;
        }

        usize tell() const { return handle->tell(); }
        usize size() const { return handle->size(); }

        void seek(usize pos) { handle->seek(pos); }

        File(FileHandle *ptr)
            : handle(ptr)
        { }
    private:
        std::unique_ptr<FileHandle> handle;
    };

    enum class Mode
    {
        read,
        write
    };

    File open(const fs::path &path, Mode mode = Mode::read);
    File stream(std::vector<svl::byte> &&bytes = {});
}