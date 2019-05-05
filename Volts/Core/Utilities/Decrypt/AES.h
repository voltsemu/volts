#pragma once

#include "Core/Collections/Array.h"

namespace Volts::AES
{
    using namespace Cthulhu;
    
    // impl of https://tools.ietf.org/html/rfc3602#section-1.1
    struct DecryptionContext
    {
        U32 Rounds;
        U32* RoundKeys;
        U32 Buffer[68];

        bool SetEncryptKey(U8* Key, U32 Len);
        bool SetDecryptKey(U8* Key, U32 Len);
        Option<Block<U8, 16>> BlockCrypt(U8 Mode, Block<U8, 16>& Input);
        Option<Array<U8>> BufferCrypt(U8 Mode, Block<U8, 16>& IV, Array<U8>& Input);
    };
}