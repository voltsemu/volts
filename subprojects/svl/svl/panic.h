#pragma once

#include <svl/macros.h>

#include <fmt/format.h>

namespace svl
{
    template<typename... T>
    [[noreturn]] void panic(const std::string& msg, T&&... args)
    {
        fmt::print(msg, args...);
        std::abort();
    }
}

#define ASSERT(expr) if (!(expr)) { svl::panic("[E0007]" STR(__FILE__) STR(__FILE__) "Assertion failed" STR(expr)); }