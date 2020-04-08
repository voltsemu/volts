#pragma once

#include "platform.h"
#include "types.h"

#include <vector>
#include <cstdint>

namespace svl::convert
{
    /**
     * @brief convert a 2 character string to a number
     * 
     * @param str the string to convert to a number
     * @return u16 the number
     */
    constexpr u16 to_u16(const char* str)
    {
        return *(u16*)str;
    }

    /**
     * @brief convert a 4 character string to a 32 bit number
     * 
     * @param str the string to convert
     * @return u32 the converted value
     */
    constexpr u32 to_u32(const char* str)
    {
        return *(u32*)str;
    }

    /**
     * @brief convert an 8 character string to a 64 bit number
     * 
     * @param str the string to convert
     * @return u64 the converted value
     */
    constexpr u64 to_u64(const char* str)
    {
        return *(u64*)str;
    }

    /**
     * @brief convert a value to a vector of bytes
     * 
     * @tparam T the type to convert
     * @param dat the data to convert
     * @return std::vector<byte> the converted type
     */
    template<typename T>
    std::vector<byte> as_bytes(T dat)
    {
        std::vector<byte> val(sizeof(T));
        memcpy(val.data(), &dat, sizeof(T));
        return val;
    }
}