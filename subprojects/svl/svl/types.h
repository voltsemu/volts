#pragma once

#include <type_traits>
#include <cstddef>

namespace svl
{
    using byte = uint8_t;

    using usize = std::make_unsigned<std::size_t>::type;
    using isize = std::make_signed<std::size_t>::type;

    template<usize N>
    using Pad = byte[N];

    template<typename... T>
    struct visitor : T...
    {
        using T::operator()...;
    };

    template<typename... T>
    visitor(T...) -> visitor<T...>;
}