#pragma once

#include <svl/platform.h>
#include <svl/types.h>

#if SYS_OSX
#   include <machine/endian.h>
#else
#   include <stdlib.h>
#endif

namespace svl::endian
{
    enum class Order
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

    inline u16 byte_swap(u16 val)
    {
#if CL_MSVC
        return _byteswap_ushort(val);
#elif CL_CLANG || CL_GCC
        return __builtin_bswap16(val);
#endif
    }

    inline u32 byte_swap(u32 val)
    {
#if CL_MSVC
        return _byteswap_ulong(val);
#elif CL_CLANG || CL_GCC
        return __builtin_bswap32(val);
#endif
    }

    inline u64 byte_swap(u64 val)
    {
#if CL_MSVC
        return _byteswap_uint64(val);
#elif CL_CLANG || CL_GCC
        return __builtin_bswap64(val);
#endif
    }

    template <typename T>
    T byte_swap(T val)
    {
        return byte_swap(reinterpret_cast<typename std::make_unsigned<T>::type>(val));
    }

    template <Order O, typename T>
    struct Value
    {
        constexpr Value() = default;
        constexpr Value(T v) : data(v) {}

        constexpr operator T() const { return get(); }

        constexpr T get() const
        {
            return (O == Order::native) ? data : byte_swap(data);
        }

        template <typename V> constexpr V as() const
        {
            static_assert(sizeof(T) == sizeof(V));
            return reinterpret_cast<V>(get());
        }

        T data;
    };

    template <typename T>
    using Big = Value<Order::big, T>;

    template <typename T>
    using Little = Value<Order::little, T>;

    template <typename T>
    using Native = Value<Order::little, T>;
} // namespace svl::endian