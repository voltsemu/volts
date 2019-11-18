#pragma once

#include "platform.h"

#include <cstdint>

namespace svl::convert
{
    constexpr std::uint16_t to_u16(const char* str)
    {
        return *(std::uint16_t*)str;
    }

    constexpr std::uint32_t to_u32(const char* str)
    {
        return *(std::uint32_t*)str;
    }

    constexpr std::uint64_t to_u64(const char* str)
    {
        return *(std::uint64_t*)str;
    }
}