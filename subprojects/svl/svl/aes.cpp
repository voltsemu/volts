#include "aes.h"

#include "macros.h"

#define GET_U32LE(A, B, C) \
    {(A) = ((u32)(B)[(C)]) \
        | ((u32)(B)[(C) + 1] << 8) \
        | ((u32)(B)[(C) + 2] << 16) \
        | ((u32)(B)[(C) + 3] << 24)}   

#define SET_U32LE(A, B, C) \
    {(B)[(C)] = (u8)(A); \
    (B)[(C) + 1] = (u8)((A) >> 8); \
    (B)[(C) + 2] = (u8)((A) >> 16); \
    (B)[(C) + 3] = (u8)((A) >> 24);}

namespace svl {
    int rounds(int size) {
        switch (size) {
        case 128: return 10;
        case 192: return 12;
        case 256: return 14;
        default: ENSURE(false);
        }
    }

    void aes::setkey_enc(const u8* key, int size) {
        nr = rounds(size);

        auto* keys = rk;

        for (int i = 0; i < (size >> 5); i++) {
            GET_U32LE(keys[i], key, i << 2);
        }

        switch (nr) {
        case 10:
            break;
        case 12:
            break;
        case 14:
            break;
        default: 
            ENSURE(false);
        }
    }

    void aes::setkey_dec(const u8* key, int size) {
        nr = rounds(size);
    }

    void aes::cbc_enc() {

    }

    void aes::cbc_dec() {

    }

    void ctr() {

    }
}
