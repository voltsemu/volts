#include "Core/Math/Bytes.h"
#include "Core/Macros.h"
#include "Core/Logger/Logger.h"
#include "AES.h"

namespace Volts
{
    using namespace Cthulhu;
    bool SupportsSSE2 = false;

    bool SupportsAES = false;

    // check if we have AES support on this CPU
    VSCRIPT({
        I32 Registers[4];
        
        CPUID(1, Registers);

        SupportsAES = Registers[2] & AES_FLAG;
        //                     ECX & 0x02000000

        SupportsSSE2 = Registers[3] & SSE2_FLAG;
        //                     EDX & 0x04000000
    })

#   define GET_UINT(B, I) ((U32)(B)[I]) | ((U32)(B)[(I) + 1] << 8) | ((U32)(B)[(I) + 2] << 16) | ((U32)(B)[(I) + 3] << 24)

    void AES::SetKeyEnc(const Cthulhu::Byte* Key, KeySize Size)
    {
        U32* RK;
        RoundKeys = RK = Buffer;

        for(U32 I = 0; I < (static_cast<U32>(Size) >> 5U); I++)
        {
            RK[I] = GET_UINT(Key, I << 2);
        }

        switch(Size)
        {
        case KeySize::S128: 
            Rounds = 10; 
            for(U32 I = 0; I < 10; I++, RK += 4)
            {

            }
            break;
        case KeySize::S256:
            Rounds = 14; 
            for(U32 I = 0; I < 7; I++, RK += 8)
            {
                
            }
            break;
        
        default: 
            return;
        }
    }

    void AES::SetKeyDec(const Cthulhu::Byte* Key, KeySize Size)
    {

    }

    void AES::EncryptCBC(Cthulhu::U32 Length, Cthulhu::Byte IV[16], const Cthulhu::Byte* Input, Cthulhu::Byte* Output)
    {

    }

    void AES::DecryptCBC(Cthulhu::U32 Length, Cthulhu::Byte IV[16], const Cthulhu::Byte* Input, Cthulhu::Byte* Output)
    {

    }

    void AES::CryptCTR(Cthulhu::U32 Length, Cthulhu::U32* Offset, const Cthulhu::Byte* Input, Cthulhu::Byte* Output)
    {

    }
}