#pragma once

#include <xmmintrin.h>
#include <emmintrin.h>

namespace Volts
{
    struct Vec128
    {
        union
        {
            __m128 Data;
            __m128i IData;
            __m128d DData;
        };
    };
}