#pragma once

#include <svl/endian.h>

namespace elf
{
    template<typename T>
    struct Header
    {
        uint32_t magic;
    };

    template<typename T>
    struct ProgHeader
    {

    };

    template<typename T>
    struct SectHeader
    {

    };
}