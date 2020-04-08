#pragma once

#include <cstdint>
#include <cstring>

#include <emmintrin.h>

namespace svl
{
    /// 8 bit signed integer
    using i8 = std::int8_t;

    /// 16 bit signed integer
    using i16 = std::int16_t;

    /// 32 bit signed integer
    using i32 = std::int32_t;

    /// 64 bit signed integer
    using i64 = std::int64_t;

    /// 8 bit unsigned integer
    using u8 = std::uint8_t;

    /// 16 bit unsigned integer
    using u16 = std::uint16_t;

    /// 32 bit unsigned integer
    using u32 = std::uint32_t;

    /// 64 bit unsigned integer
    using u64 = std::uint64_t;

    /// 8 bit unsigned integer
    using byte = std::uint8_t;

    template<std::size_t N = 1>
    using padding = byte[N];

    /// 32 bit floating point number
    using f32 = float;

    /// 64 bit double presicion floating point number
    using f64 = double;
    
    /**
     * @brief 128 bit vector type
     * 
     */
    union v128
    {
        /// 128 bits of packed integers
        __m128i ints;

        /// 128 bits of packed doubles
        __m128d doubles;
        struct
        {
            /// the low 64 bits
            u64 lo;

            /// the high 64 bits
            u64 hi;
        };
    };

    static_assert(sizeof(v128) == 16);
}