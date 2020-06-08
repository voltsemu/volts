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

    template<typename T>
    T bswap(T val)
    {
        if constexpr (sizeof(T) == 2)
        {
#if CL_MSVC
            return _byteswap_ushort(val);
#else
            return __builtin_bswap16(val);
#endif
        }
        else if constexpr (sizeof(T) == 4)
        {
#if CL_MSVC
            return _byteswap_ulong(val);
#else
            return __builtin_bswap32(val);
#endif
        }
        else if constexpr (sizeof(T) == 8)
        {
#if CL_MSVC
            return _byteswap_uint64(val);
#else
            return __builtin_bswap64(val);
#endif
        }

        static_assert(sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8);
    }

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