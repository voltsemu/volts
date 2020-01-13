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
    std::uint16_t byte_swap(std::uint16_t val);

    std::uint32_t byte_swap(std::uint32_t val);

    std::uint64_t byte_swap(std::uint64_t val);

    template<typename T>
    T byte_swap(T val)
    {
        return byte_swap(*reinterpret_cast<typename std::make_unsigned<T>::type*>(&val));
    }

    enum class endian_order
    {
#if SYS_WINDOWS
        big = REG_DWORD_BIG_ENDIAN,
        little = REG_DWORD_LITTLE_ENDIAN,
        native = REG_DWORD,
#else
        big = __ORDER_BIG_ENDIAN__,
        little = __ORDER_LITTLE_ENDIAN__,
        native = __BYTE_ORDER__,
#endif
    };

    template<typename T, endian_order order>
    struct endian_value
    {
        constexpr operator T() const 
        { 
            return get(); 
        }

        constexpr T get() const
        {
            return (order == endian_order::native) ? val : byte_swap(val);
        }

        T val;
    };

    template<typename T>
    using big = endian_value<T, endian_order::big>;

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