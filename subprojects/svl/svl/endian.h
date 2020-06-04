#pragma once

#include <svl/platform.h>

namespace svl
{
    enum class ByteOrder
    {
#if SYS_WINDOWS
        big = REG_DWORD_BIG_ENDIAN,
        little = REG_DWORD_LITTLE_ENDIAN,
        native = REG_DWORD,
#else
        big = __ORDER_BIG_ENDIAN__,
        little = __ORDER_LITTLE_ENDIAN__,
        native = __BYTE_ORDER__,
#endif
    };

    template<typename T, ByteOrder Order>
    struct EndianValue
    {
        constexpr EndianValue() = default;
        constexpr EndianValue(const EndianValue&) = default;
        constexpr EndianValue(T v) : val(v) { }

        constexpr operator T() const { return get(); }

        constexpr T get() const { return (Order == ByteOrder::native) ? val : bswap(val); }

        template<typename V>
        constexpr V as() const
        {
            static_assert(sizeof(T) == sizeof(V));
            return (V)get();
        }

        T val;
    };

    template<typename T>
    using Big = EndianValue<T, ByteOrder::big>;

    template<typename T>
    using Little = EndianValue<T, ByteOrder::little>;

    template<typename T>
    using Native = EndianValue<T, ByteOrder::native>;
}