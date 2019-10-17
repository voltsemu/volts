#pragma once

#include <Meta/Aliases.h>

namespace Volts
{
    // Justification:
    //  - Q: What are these functions even for?
    //  - A: The PS3 file structure relies alot on magic numbers such as
    //       \0PSF or SFO\0 to denote the type of a file or the type
    //       of a header inside a file. String comparison is much slower
    //       than integer comparison, so if at all possible we should
    //       create an integer at compile time using constexpr functions
    //       to improve runtime performance.

    // convert 2 characters into a number ie "AB"
    constexpr Cthulhu::U16 operator ""_U16(const char* Str, size_t)
    {
        // turn a string into a number by casting the pointer to
        // the relevant type at compile time and then returning
        // type number by value rather than reference to duplicate it
        return *(Cthulhu::U16*)Str;
    }

    // convert 4 characters into a number ie "SFO\0"
    constexpr Cthulhu::U32 operator ""_U32(const char* Str, size_t)
    {
        return *(Cthulhu::U32*)Str;
    }

    // convert 8 characters into a number, not often used for magic, but here for completions sake
    constexpr Cthulhu::U64 operator ""_U64(const char* Str, size_t)
    {
        return *(Cthulhu::U64*)Str;
    }
}