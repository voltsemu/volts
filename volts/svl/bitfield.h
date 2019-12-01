#pragma once

#include <cstddef>
#include <cmath>

namespace svl
{
    template<typename T, std::size_t Tstart, std::size_t Tend>
    struct bit_field
    {
        T get() const
        {
            return (val << Tstart) & ((1 << (Tend - Tstart)) - 1);
        };

        operator T() const { return get(); }

    private:
        T val;
    };
}