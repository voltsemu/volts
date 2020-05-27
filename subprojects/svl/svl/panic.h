#pragma once

#include <string>

namespace svl
{
    template<typename... T>
    [[noreturn]] void panic(const std::string& fmt, T... args)
    {
        (void)fmt;
        (void)args...;
        // TODO: we need spdlog or fmt or something
        printf("uhhh\n");
        while (true);
    }
}