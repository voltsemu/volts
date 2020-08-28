#pragma once

#include <cstdint>
#include <memory>

#include "macros.h"

namespace svl {
    using u8 = std::uint8_t;
    using u16 = std::uint16_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;

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

    template<typename T, order O>
    struct val {
        val() : data(0) { }
        val(T v) : data(v) { }

        T get() const { return (O == order::native) ? data : bswap(data); }

        operator T() const { return get(); }

        T data;
    };

    template<typename T>
    using big = val<T, order::big>;

    template<typename T>
    using little = val<T, order::little>;
}
