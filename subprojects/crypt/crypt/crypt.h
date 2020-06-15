#pragma once

#include <stdint.h>
#include <wmmintrin.h>

namespace crypt
{
    enum class Mode { enc, dec };

    template<int I>
    struct Aes
    {
        static Aes<I> enc()
        {

        }

        static Aes<I> dec()
        {

        }

        template<Mode M>
        void ecb()
        {

        }

        template<Mode M>
        void cbc()
        {

        }

        void ctr()
        {

        }

    private:
        constexpr int rounds() const
        {
            switch (I)
            {
            case 128: return 10;
            case 192: return 12;
            case 256: return 14;
            }
        }
    };
}