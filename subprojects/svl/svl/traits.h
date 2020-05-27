#pragma once

namespace svl
{
    template<typename... T>
    struct Visitor : T...
    {
        using T::operator()...;
    };

    template<typename... T>
    Visitor(T...) -> Visitor<T...>;
}