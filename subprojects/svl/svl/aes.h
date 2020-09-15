#pragma once

#include "types.h"

namespace svl {
    struct aes {
        aes() {}

        int nr;
        u32* rk;
        u32 buf[68];

        void setkey_enc(const u8* key, int size);
        void setkey_dec(const u8* key, int size);

        void cbc_enc();
        void cbc_dec();

        void ctr();
    };
}