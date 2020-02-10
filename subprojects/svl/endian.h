#pragma once

#include "platform.h"
#include <cstdint>
#include <type_traits>
#include <spdlog/spdlog.h>

#if SYS_OSX
#   include <machine/endian.h>
#else
#   include <stdlib.h>
#endif

#if SYS_WINDOWS
#   include <Windows.h>
#endif

namespace svl::endian
{
    /**
     * @brief byteswap a 16 bit value
     * 
     * @param val the value to byte swap
     * @return std::uint16_t the byte swapped value
     */
    std::uint16_t byte_swap(std::uint16_t val);

    /**
     * @brief byteswap a 32 bit value
     * 
     * @param val the value to byte swap
     * @return std::uint32_t the byte swapped value
     */
    std::uint32_t byte_swap(std::uint32_t val);

    /**
     * @brief byteswap a 64 bit value
     * 
     * @param val the value to byte swap
     * @return std::uint64_t the byte swapped value
     */
    std::uint64_t byte_swap(std::uint64_t val);

    /**
     * @brief byteswap a generic type thats either 2, 4 or 8 bytes wide
     * 
     * @tparam T the type to byteswap
     * @param val the value to byteswap
     * @return T the byteswapped type
     */
    template<typename T>
    T byte_swap(T val)
    {
        return byte_swap(*reinterpret_cast<typename std::make_unsigned<T>::type*>(&val));
    }

    /**
     * @brief an enum that represents a byte order
     * 
     */
    enum class endian_order
    {
#if SYS_WINDOWS
        /// big endian byte order (ppc64)
        big = REG_DWORD_BIG_ENDIAN,

        /// little endian byte order (x86_64)
        little = REG_DWORD_LITTLE_ENDIAN,

        /// byte order of the current platform
        native = REG_DWORD,
#else
        /// big endian byte order (ppc64)
        big = __ORDER_BIG_ENDIAN__,

        /// little endian byte order (x86_64)
        little = __ORDER_LITTLE_ENDIAN__,

        /// byte order of the current platform
        native = __BYTE_ORDER__,
#endif
    };

    /**
     * @brief an endian type 
     * 
     * a type that can correct its byte order when read
     * so when assigned a big endian value it can convert it to the native endian when read back
     * 
     * @tparam T the type of the value
     * @tparam order the desired byte order of the value
     */
    template<typename T, endian_order order>
    struct endian_value
    {
        /**
         * @brief get the value converted to the platforms native byte ordering
         * @see endian_value::get
         * @return T the value
         */
        constexpr operator T() const 
        { 
            return get(); 
        }

        /**
         * @brief get the value converted to the platforms byte ordering
         * 
         * @return constexpr T the converted value
         */
        constexpr T get() const
        {
            return (order == endian_order::native) ? val : byte_swap(val);
        }

        /// the native value
        T val;
    };

    /**
     * @brief a big endian value
     * 
     * @tparam T the type of the value
     */
    template<typename T>
    using big = endian_value<T, endian_order::big>;

    /**
     * @brief a little endian value
     * 
     * @tparam T the type of the value
     */
    template<typename T>
    using little = endian_value<T, endian_order::little>;
} 

template<typename T>
struct fmt::formatter<svl::endian::big<T>> : fmt::formatter<T> 
{
    auto format(svl::endian::big<T> val, format_context& ctx) 
    {
        return fmt::formatter<T>::format(val.get(), ctx);
    }
};

template<typename T>
struct fmt::formatter<svl::endian::little<T>> : fmt::formatter<T> 
{
    auto format(svl::endian::little<T> val, format_context& ctx) 
    {
        return fmt::formatter<T>::format(val.get(), ctx);
    }
};