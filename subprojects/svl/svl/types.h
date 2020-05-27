#pragma once

#include <cstdint>
#include <cstddef>
#include <type_traits>

namespace svl
{
    using u8 = std::uint8_t;
    using u16 = std::uint16_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;

    using i8 = std::int8_t;
    using i16 = std::int16_t;
    using i32 = std::int32_t;
    using i64 = std::int64_t;

    using byte = u8;

    using usize = std::make_unsigned<std::size_t>::type;
    using isize = std::make_signed<std::size_t>::type;

    template<usize N>
    using pad = byte[N];
}