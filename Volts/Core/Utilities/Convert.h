#pragma once

#include <Meta/Aliases.h>

namespace Volts
{
    constexpr Cthulhu::U16 operator ""_U16(const char* Str, unsigned long)
    {
        return *(Cthulhu::U16*)Str;
    }

    constexpr Cthulhu::U32 operator ""_U32(const char* Str, unsigned long)
    {
        return *(Cthulhu::U32*)Str;
    }

    constexpr Cthulhu::U64 operator ""_U64(const char* Str, unsigned long)
    { 
        return *(Cthulhu::U64*)Str;
    }
}