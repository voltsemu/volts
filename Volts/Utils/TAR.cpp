#include "TAR.h"

#include <Core/Collections/CthulhuString.h>

namespace Volts::Utils
{
    using namespace Cthulhu;

    I32 OctalToDecimal(I32 Num)
    {
        I32 Ret = 0, I = 0, Rem;
        while(Num != 0)
        {
            Rem = Num % 10;
            Num /= 10;
            Ret += Rem * (I32)pow(8, I++);
        }
        return Ret;
    }

    // ustar\20
    Byte USTAR[] = { 0x75, 0x73, 0x74, 0x61, 0x72, 0x20 };

    TAR::Object LoadTAR(Binary B)
    {
        // pain and suffering
        B.Seek(0);
        TAR::Object Ret;
        Ret.File = B;

        for(;;)
        {
            // read in the header
            auto Head = B.Read<TAR::Header>();

            // we break at the first invalid header (technically all files are valid tar files)
            if(memcmp(Head.Magic, USTAR, sizeof(USTAR)) != 0)
                return Ret;

            // tar file size is stored as an octal number string rather than a normal 8 byte number
            // so we use this to get the actual number we need
            // (fuck you whoever decided that was a good idea)
            I32 Size = OctalToDecimal(Cthulhu::Utils::ParseInt(Head.Size).Get());

            // all TAR file headers are aligned to 512 bytes absolute offset
            // so we round up to the next 512 byte boundary
            I32 Aligned = (Size + B.Tell() + 512 - 1) & ~(512 - 1);

            Ret.Offsets[(char*)Head.Name] = B.Tell();

            B.Seek(Aligned);
        }

        return Ret;
    }
}