#pragma once

#include <svl/types.h>

namespace crypt
{
    void init();

    namespace aes
    {
        enum class Algo
        {
            ecb,
            ctr,
            cbc
        };

        enum class Mode
        {
            enc,
            dec
        };

        template<Algo A, svl::usize N>
        struct Context
        {
            template<Mode M>
            Context(const uint8_t* key)
            {
                if constexpr (M == Mode::enc)
                {

                }
                else
                {

                }
            }


        private:
            uint32_t buf[68];
            static_assert(N == 128 || N == 192 || N == 256);
            constexpr usize rounds()
            {
                if (N == 128)
                    return 10;
                else if (N == 192)
                    return 12;
                else
                    return 14;
            }
        };
    }
}