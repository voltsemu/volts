#pragma once

#include <cstddef>

namespace svl
{
    /**
     * @brief a range of bits starting at I with N bits of length
     * 
     * @tparam T the underlying type
     * @tparam I the starting offset of the range
     * @tparam N the number of bits
     */
    template<typename T, std::size_t I, std::size_t N>
    struct bitrange
    {
        bitrange() = default;
        bitrange(T v)
            : val(v)
        {}

        static constexpr T mask = ~((1 << N) - 1) << I;
        void write(T data)
        {
            val &= mask;
            val |= (data << N);
        }

        T read() const
        {
            // extract I bits from K position
            return (((1 << N) - 1) & (val >> (I - 1)));
        }

        operator T() const { return read(); }

        T operator=(T data) 
        {
            write(data);
            return read();
        }

    private:
        T val;
    };
}