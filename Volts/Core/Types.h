#pragma once

#include <Meta/Aliases.h>

#include <xmmintrin.h>
#include <emmintrin.h>

namespace Volts::Types
{
    using namespace Cthulhu;

    union V128
    {
        Byte Bytes[16];
        __m128 VF;
        __m128i VI;
        __m128d VD;
    };

    static_assert(sizeof(V128) == 16);
}