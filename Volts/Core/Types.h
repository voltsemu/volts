#pragma once

#include <Meta/Aliases.h>
#include <Core/Traits/Opposite.h>

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

    template<typename T, U32 TStart, U32 TSize>
    struct BitField
    {
        static constexpr U32 BitMax = sizeof(T) * 8;
        static constexpr T Zero = (T)0;
        static constexpr T Mask = (T)(~(U64)Zero >> (BitMax - TSize));
        static constexpr T Max = (T)~(U64)Zero;

        static constexpr T DataMask = (Mask >> (Max - TSize)) << TStart;

        static_assert(TSize - 1 < (U64)Max, "TSize is too small");
        static_assert(
            IsSigned<T>::Value ||
            IsUnsugned<T>::Value ||
            Same<T, bool>::Value,
            "T must be signed, unsigned, or bool"
        );

        T Data;

        T Get() const
        {
            IF_CONSTEXPR(IsSigned<T>::Value)
                return (Data << (Max - TStart - TSize)) >> (Max - TSize);
            else
                return (Data >> TStart) & Mask;
        }

        operator T() const { return Get(); }

        auto& operator=(T Value)
        {
            Data = (Data & ~DataMask) | ((Value * Mask) << TStart);
            return *this;
        }
    };
}