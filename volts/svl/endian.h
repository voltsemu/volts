#pragma once

#include "platform.h"
#include <cstdint>
#include <type_traits>

#if SYS_OSX
#   include <machine/endian.h>
#else
#   include <stdlib.h>
#endif


namespace svl::endian
{
    std::uint16_t byte_swap(std::uint16_t val);

    std::uint32_t byte_swap(std::uint32_t val);

    std::uint64_t byte_swap(std::uint64_t val);

    template<typename T>
    T byte_swap(T val)
    {
        return byte_swap(reinterpret_cast<typename std::make_unsigned<T>::type>(val));
    }

    template<typename T>
    struct big
    {
        operator T() const 
        { 
            return get(); 
        }

        T get() const
        {
            // we only support little endian platforms so this will do
            return byte_swap(val);
        }

        T val;
    };
} 
