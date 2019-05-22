#pragma once

#include "Meta/Aliases.h"
#include <xmmintrin.h>
#include <emmintrin.h>

namespace Volts
{
    enum class Instruction : Cthulhu::U64
    {
        MMX     = (1 << 0), //
        SSE     = (1 << 1), //
        SSE2    = (1 << 2), //
        SSE3    = (1 << 3), //
        SSSE3   = (1 << 4), //
        SSE4A   = (1 << 5), //
        SSE41   = (1 << 6), //
        SSE42   = (1 << 7), //
        AVX     = (1 << 8), //
        AVX2    = (1 << 9), //
        AVX512  = (1 << 10),
        AES     = (1 << 11), //
        VAES    = (1 << 12), //
        TSX     = (1 << 13), //
        HLE     = (1 << 14), //
        TSC     = (1 << 15), //
        RTM     = (1 << 16), //
        SHA     = (1 << 17), //
    };

    inline Instruction operator|(const Instruction L, const Instruction R)
    {
        return static_cast<Instruction>(static_cast<Cthulhu::U64>(L) | static_cast<Cthulhu::U64>(R));
    }

    inline Instruction& operator|=(Instruction& L, const Instruction R)
    {
        return L = L | R;
    }

    inline Instruction operator&(const Instruction L, const Instruction R)
    {
        return static_cast<Instruction>(static_cast<Cthulhu::U64>(L) & static_cast<Cthulhu::U64>(R));
    }

    inline Instruction& operator&=(Instruction& L, const Instruction R)
    {
        return L = L & R;
    }

    inline bool operator!(Instruction I)
    {
        return *reinterpret_cast<Cthulhu::U64*>(&I) == 0;
    }

    Instruction SupportedInstructions();

    struct Vec128
    {
        Vec128(__m128 D) : Data(D) {}
        Vec128(__m128i D) : IData(D) {}
        Vec128(__m128d D) : DData(D) {}
        
        union
        {
            __m128 Data;
            __m128i IData;
            __m128d DData;
        };

        Vec128 operator^(const Vec128 Other) const;
    };
}