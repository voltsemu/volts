#pragma once

#include "platform.h"

#include <cstdint>
#include <type_traits>

#if SYS_OSX
#   include <machine/endian.h>
#else
#   include <stdlib.h>
#endif

namespace svl::convert
{
    constexpr std::uint16_t to_u16(const char* str)
    {
        return *(std::uint16_t*)str;
    }

    constexpr std::uint32_t to_u32(const char* str)
    {
        return *(std::uint32_t*)str;
    }

    constexpr std::uint64_t to_u64(const char* str)
    {
        return *(std::uint64_t*)str;
    }

    inline std::uint16_t byte_swap(std::uint16_t val)
    {
#if CL_MSVC
        return _byteswap_ushort(val);
#elif CL_CLANG || CL_GNU
        return __builtin_bswap16(val);
#endif
    }

    inline std::uint32_t byte_swap(std::uint32_t val)
    {
#if CL_MSVC
        return _byteswap_ulong(val);
#elif CL_CLANG || CL_GNU
        return __builtin_bswap32(val);
#endif
    }

    inline std::uint64_t byte_swap(std::uint64_t val)
    {
#if CL_MSVC
        return _byteswap_uint64(val);
#elif CL_CLANG || CL_GNU
        return __builtin_bswap64(val);
#endif
    }

    template<typename T>
    T byte_swap(T val)
    {
        return byte_swap(reinterpret_cast<typename std::make_unsigned<T>::type>(val));
    }
}