#pragma once

#include <svl/types.h>
#include <vector>
#include <string>

namespace svl::convert
{
    template<usize N>
    constexpr u16 to_u16(const char(&str)[N])
    {
        static_assert(N >= 2);
        return *(u16*)str;
    }

    template<usize N>
    constexpr u32 to_u32(const char(&str)[N])
    {
        static_assert(N >= 4);
        return *(u32*)str;
    }

    template<usize N>
    constexpr u64 to_u64(const char(&str)[N])
    {
        static_assert(N >= 8);
        return *(u64*)str;
    }

    template<typename T>
    std::string to_string(const std::vector<T>& vec)
    {
        std::string out = "[ ";

        for (size_t i = 0; i < vec.size(); i++)
        {
            if (i != 0)
            {
                out += ", ";
            }

            out += std::to_string(vec[i]);
        }

        out += " ]";

        return out;
    }
}
