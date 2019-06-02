#pragma once

#include <Meta/Aliases.h>
#include <Core/Traits/Opposite.h>

namespace Volts
{
    template<typename T, Cthulhu::U32 TStart, Cthulhu::U32 TSize>
    struct BitField
    {
        static constexpr Cthulhu::U32 BitMax = sizeof(T) * 8;
        static constexpr T Zero = (T)0;
        static constexpr T Mask = (T)(~(Cthulhu::U64)Zero >> (BitMax - TSize));
        static constexpr T Max = (T)~(Cthulhu::U64)Zero;
        constexpr T DataMask()
        {
            return (Mask >> (Max - TSize)) << TStart;
        }

        static_assert(TSize - 1 < (Cthulhu::U64)Max, "TSize is too small");
        static_assert(
            Cthulhu::IsSigned<T>::Value ||
            Cthulhu::IsUnsigned<T>::Value ||
            Cthulhu::Same<T, bool>::Value,
            "T must be signed, unsigned or a bool"
        );

        T Data;

        T Get() const
        {
            IF_CONSTEXPR(Cthulhu::IsSigned<T>::Value)
                return (Data << (Max - TStart - TSize)) >> (Max - TSize);
            else
                return (Data >> TStart) & Mask;
        }

        operator T() const { return Get(); }

        auto& operator=(T Value)
        {
            Data = ((Data & ~DataMask()) | ((Value & Mask) << TStart));
            return *this;
        }
    };
}