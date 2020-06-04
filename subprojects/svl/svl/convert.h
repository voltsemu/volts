#pragma once

#include <svl/types.h>
#include <stdint.h>

namespace svl::convert
{
    template<usize N>
    constexpr uint16_t u16(const char(&str)[N])
    {
        static_assert(N >= 2);
        return *(uint16_t*)str;
    }

    template<std::size_t N>
    constexpr uint32_t u32(const char(&str)[N])
    {
        static_assert(N >= 4);
        return *(uint32_t*)str;
    }

    template<std::size_t N>
    constexpr uint64_t u64(const char(&str)[N])
    {
        static_assert(N >= 8);
        return *(uint64_t*)str;
    }
}