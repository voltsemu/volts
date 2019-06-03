#pragma once

#include "Meta/Aliases.h"

namespace Volts
{
    // we only support 128 and 256 bit decryption for now
    enum class KeySize : Cthulhu::U32
    {
        S128 = 128,
        S256 = 256,
    };

    struct AES
    {
        Cthulhu::U8 Rounds;
        Cthulhu::U32* RoundKeys;
        Cthulhu::U32 Buffer[68];

        AES& SetKeyEnc(const Cthulhu::Byte* Key, KeySize Size);
        AES& SetKeyDec(const Cthulhu::Byte* Key, KeySize Size);

        const AES& EncryptECB(const Cthulhu::Byte Input[16], Cthulhu::Byte Output[16]) const;
        const AES& DecryptECB(const Cthulhu::Byte Input[16], Cthulhu::Byte Output[16]) const;

        const AES& EncryptCBC(Cthulhu::U32 Length, Cthulhu::Byte IV[16], const Cthulhu::Byte* Input, Cthulhu::Byte* Output) const;
        const AES& DecryptCBC(Cthulhu::U32 Length, Cthulhu::Byte IV[16], const Cthulhu::Byte* Input, Cthulhu::Byte* Output) const;

        const AES& CryptCTR(Cthulhu::U32 Length, Cthulhu::U32* Offset, const Cthulhu::Byte* Input, Cthulhu::Byte* Output) const;
    };
}