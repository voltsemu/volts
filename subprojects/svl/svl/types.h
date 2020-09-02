#pragma once

#include <cstdint>
#include <memory>
#include <type_traits>
#include <optional>

#include "macros.h"

namespace svl {
    using u8 = std::uint8_t;
    using u16 = std::uint16_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;

    using usize = typename std::make_unsigned<size_t>::type;

    template<typename T>
    using unique = std::unique_ptr<T>;

    enum class order {
#if SYS_WIN
        big = REG_DWORD_BIG_ENDIAN,
        little = REG_DWORD_LITTLE_ENDIAN,
        native = REG_DWORD
#else
        big = __ORDER_BIG_ENDIAN__,
        little = __ORDER_LITTLE_ENDIAN__,
        native = __BYTE_ORDER__
#endif
    };

#if SYS_WIN
#   define BSWAP16 _byteswap_ushort
#   define BSWAP32 _byteswap_ulong
#   define BSWAP64 _byteswap_uint64
#else
#   define BSWAP16 __builtin_bswap16
#   define BSWAP32 __builtin_bswap32
#   define BSWAP64 __builtin_bswap64
#endif

    template<typename T>
    constexpr bool is_bswap_v =
        (sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8)
        && (std::is_standard_layout_v<T> && std::is_trivial_v<T>);

    template<typename T>
    auto bswap(T val) {
        static_assert(is_bswap_v<T>);
        using utype = typename std::make_unsigned<T>::type;

        if constexpr (sizeof(T) == 2) {
            return (T)BSWAP16((utype)val);
        } else if constexpr (sizeof(T) == 4) {
            return (T)BSWAP32((utype)val);
        } else if constexpr (sizeof(T) == 8) {
            return (T)BSWAP64((utype)val);
        }
    }

    template<typename T, order O>
    struct val {
        static_assert(is_bswap_v<T>);

        val() : val((T)0) { }
        val(T v, order o = order::native) : data(o == O ? v : bswap(v)) { }

        T get() const {
            return (O == order::native)
                ? data
                : bswap(data);
        }

        operator T() const { return get(); }

        T data;
    };

    template<typename T>
    using big = val<T, order::big>;

    template<typename T>
    using little = val<T, order::little>;

    template<typename... T>
    struct visitor : T... {
        using T::operator()...;
    };

    template<typename... T>
    visitor(T...) -> visitor<T...>;
}
