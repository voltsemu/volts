#pragma once

#include <type_traits>
#include <cstddef>

namespace svl
{
    using usize = std::make_unsigned<std::size_t>::type;
    using isize = std::make_signed<std::size_t>::type;

    template<typename... T>
    struct visitor : T...
    {
        using T::operator()...;
    };

    template<typename... T>
    visitor(T...) -> visitor<T...>;
}