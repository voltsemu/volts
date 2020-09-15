#include "aes.h"

#include "macros.h"

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
