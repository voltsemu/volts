#pragma once

#include <svl/types.h>
#include <svl/macros.h>
#include <svl/logger.h>
#include <stdint.h>

#include <wmmintrin.h>

namespace crypt
{
    void init();

    enum class Mode { enc, dec };

    namespace aes
    {
        template<svl::usize N>
        struct Context
        {
            static Context enc(const uint8_t* key)
            {

            }

            static Context dec(const uint8_t* key)
            {

            }

            template<Mode M>
            void crypt_ecb(uint8_t in[16], uint8_t out[16])
            {

            }

            template<Mode M>
            void crypt_cbc(svl::usize len, uint8_t iv[16], const uint8_t* in, uint8_t* out)
            {
                ENSURE(len % 16);

                if constexpr (M == Mode::dec)
                {
                    uint8_t temp[16];
                    while (len > 0)
                    {
                        memcpy(temp, in, 16);
                        crypt_ecb<M>(in, out);

                        for (int i = 0; i < 16; i++)
                            out[i] ^= iv[i];

                        memcpy(iv, temp, 16);

                        in += 16;
                        out += 16;
                        len -= 16;
                    }
                }
                else
                {
                    while (len > 0)
                    {
                        for (int i = 0; i < 16; i++)
                            out[i] = in[i] ^ iv[i];

                        crypt_ecb<M>(out, out);
                        memcpy(iv, out, 16);

                        in += 16;
                        out += 16;
                        len -= 16;
                    }
                }
            }

            void crypt_ctr(svl::usize len, svl::usize* off, uint8_t nonce[16], uint8_t block[16], const uint8_t* in, uint8_t* out)
            {
                auto n = *off;

                while (len--)
                {
                    if (n == 0)
                    {
                        crypt_ecb(nonce, block);

                        for (int i = 16; i > 0; i--)
                            if (++nonce[i - 1] != 0)
                                break;
                    }

                    *out++ = ((*in++) ^ block[n]);

                    n = (n + 1) & 0x0F;
                }

                *off = n;
            }

        private:
            uint32_t buf[68];

            static_assert(N == 128 || N == 192 || N == 256);

            constexpr svl::usize rounds()
            {
                if (N == 128)
                    return 10;
                else if (N == 192)
                    return 12;
                else if (N == 256)
                    return 14;
            }
        };
    }
}