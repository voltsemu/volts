#pragma once

#include "types.h"

namespace svl {
    struct aes {
        int nr;
        u32* rk;
        u32 buf[68];

        void setkey_enc();
        void setkey_dec();

        void cbc();
        void ctr();
    };
}