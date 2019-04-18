#pragma once

#include <Core/Math/Bytes.h>

namespace RPCS3X
{
    enum class Endian : U8
    {
        Big,
        Little,
    };

    template<typename T, Endian Order>
    struct Value
    {
        static_assert(sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8, "It is only possible to byteswap values that are 2, 4, or 8 bytes wide");
        T Data;
        Value& operator=(T Other) { Data = Other; }

        //so this is strange, but this gets the data as little endian so it works on x86
        T Get() const
        {
            //the stored data is little endian
            if constexpr(Order == Endian::Little) 
            {
                return Data;
            }
            //the stored data is big endian
            else
            {
                return Math::ByteSwap<T>(Data);
            }
        }
    };

    template<typename T>
    using Little = Value<T, Endian::Little>;

    template<typename T>
    using Big = Value<T, Endian::Big>;
}