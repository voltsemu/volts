#pragma once

#include <Meta/Aliases.h>
#include <Core/Traits/Opposite.h>

namespace Volts
{
    // represents a range of bits in a type
    template<typename T, Cthulhu::U32 TStart, Cthulhu::U32 TSize>
    struct BitField
    {
        // the amount of bits this type has
        static constexpr Cthulhu::U32 BitMax = sizeof(T) * 8;

        // the zero of this type
        static constexpr T Zero = (T)0;

        // the mask to use to get data out
        static constexpr T Mask = (T)(~(Cthulhu::U64)Zero >> (BitMax - TSize));

        // the max value this type can have
        static constexpr T Max = (T)~(Cthulhu::U64)Zero;

        // the data mask for this type
        constexpr T DataMask()
        {
            return (Mask >> (Max - TSize)) << TStart;
        }

        // make sure everything is in range
        static_assert(TSize - 1 < (Cthulhu::U64)Max, "TSize is too small");
        static_assert(
            Cthulhu::IsSigned<T>::Value ||
            Cthulhu::IsUnsigned<T>::Value ||
            Cthulhu::Same<T, bool>::Value,
            "T must be signed, unsigned or a bool"
        );

        // the actual data
        T Data;

        // grab the needed bits and turn them into data
        T Get() const
        {
            // signed requires different shifting
            IF_CONSTEXPR(Cthulhu::IsSigned<T>::Value)
                return (Data << (Max - TStart - TSize)) >> (Max - TSize);
            else
                return (Data >> TStart) & Mask;
        }

        // for implicit casts
        operator T() const { return Get(); }

        // set the field to different data
        auto& operator=(T Value)
        {
            Data = ((Data & ~DataMask()) | ((Value & Mask) << TStart));
            return *this;
        }
    };
}