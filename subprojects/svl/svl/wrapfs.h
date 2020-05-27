#pragma once

#if __has_include(<filesystem>)
#   include <filesystem>
namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
#   include <experimental/filesystem>
namespace fs = std::experimental::filesytem;
#else
#   error "<filesystem> support missing"
#endif

#include <fmt/format.h>

template<>
struct fmt::formatter<fs::path>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template<typename T>
    auto format(const fs::path& path, T& ctx)
    {
        return format_to(ctx.out(), "{}", path.string());
    }
};