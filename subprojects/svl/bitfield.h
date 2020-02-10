#pragma once

#include <cstddef>
#include <cmath>

namespace svl
{
    /**
     * @brief represents a bit field at an offset in a value
     * 
     * @tparam T the type of this bit field
     * @tparam Tstart the bit to start at
     * @tparam Tend the bit to end at
     */
    template<typename T, std::size_t Tstart, std::size_t Tend>
    struct bit_field
    {
        /**
         * @brief convert the value from bits to T
         * 
         * @return T the bitfield value
         */
        T get() const
        {
            return (val << Tstart) & ((1 << (Tend - Tstart)) - 1);
        };

        /**
         * @brief convert the value from bits to T
         * 
         * 
         * @see bit_field::get
         * @return T the bitfield value
         */
        operator T() const { return get(); }

    private:
        /// the underlying value
        T val;
    };
}