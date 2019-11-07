#include "TAR.h"

#include <Core/Collections/CthulhuString.h>

namespace Volts::Utils
{
    I32 OctalToDecimal(I32 Num)
    {
        I32 Ret = 0, I = 0, Rem;
        while(Num != 0)
        {
            Rem = Num % 10;
            Num /= 10;
            Ret += Rem * pow(8, I++);
        }
        return Ret;
    }

    std::optional<TAR::Object> LoadTAR(Binary& B)
    {
        TAR::Object Ret;
        while(B.Tell() < B.Len())
        {
            VINFO("Depth = {}", B.Tell());
            auto Head = B.Read<TAR::Header>();
            if(Memory::Compare<Byte>(Head.Magic, (const Byte*)"ustar\0", 6) != 0)
            {
                VERROR("Invalid TAR Magic");
                return std::nullopt;
            }

            I32 Size = OctalToDecimal(Cthulhu::Utils::ParseInt(Head.Size).Get());
            Ret.Offsets[(char*)Head.Name] = B.Tell() - sizeof(TAR::Header);
            B.Seek(Size);
        }


        return std::nullopt;
    }
}