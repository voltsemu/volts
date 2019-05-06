#pragma once

#include <Meta/Macros.h>
#include <Core/Math/Bytes.h>

namespace Volts
{
    // what endianness does the data we're loading have?
    enum class Endian : Cthulhu::U8
    {
        // powerpc is big endian
        Big,
        // x86 and arm are little endian
        Little,
    };
    
    // Justification:
    //  - Q: Why have an entire class to represent a big or little endian value?
    //  - A: Due to the way the emulator is designed, many types are constructed through Memory::Copy.
    //       because of this and the way C++ compilers lay out their structs in memory it is possible
    //       to construct types without constructors very easily.
    //       Furthermore this allows for values to be converted to litlle endian only when needed
    //       to increase performance by reducing calls to byteswap functions.
    //

    /**
     * @brief 
     * 
     * @tparam T The type to store in the value
     * @tparam Order the order of the bytes copied into the value
     */
    template<typename T, Endian Order>
    struct Value
    {
        // always check the constraints
        static_assert(sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8, "It is only possible to byteswap values that are 2, 4, or 8 bytes wide");
        
        // the stored data
        T Data;

#ifdef PLATFORM_LITTLE_ENDIAN
        // so this is strange, but this gets the data as little endian so it works on x86 which is little endian only
        T Get() const
        {
            // check if this value is loaded as Little endian Data
            if constexpr(Order == Endian::Little) 
            {
                // if it is we can just return the data as target is little endian as well
                return Data;
            }
            // if the data is loaded as big endian
            else
            {
                // then we need to byteswap the data so it becomes little endian and is usable by target
                return Cthulhu::Math::GenericByteSwap<T>(Data);
            }
        }
#else
        T Get() const
        {
            if constexpr (Order == Endian::Little)
            {
                return Cthulhu::Math::GenericByteSwap<T>(Data);
            }
            else
            {
                return Data;
            }
        }
#endif

        operator T() const { return Get(); }
    };

    // alias for little endian types
    template<typename T>
    using Little = Value<T, Endian::Little>;

    // alias for big endian types
    template<typename T>
    using Big = Value<T, Endian::Big>;
}