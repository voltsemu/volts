#pragma once

#include <Meta/Aliases.h>

namespace RPCS3X
{
    constexpr U16 ""_U16(const char* Str)
    {
        return *reinterpret_cast<U16*>(Str);
    }

    constexpr U32 ""_U32(const char* Str)
    {
        return *reinterpret_cast<U32*>(Str);
    }

    constexpr U64 ""_U64(const char* Str)
    {
        return *reinterpret_cast<U64*>(Str);
    }
}