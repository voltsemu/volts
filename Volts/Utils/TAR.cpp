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

    std::vector<std::string> TAR::Object::FileNames()
    {
        std::vector<std::string> Names;

        for(auto& [Key, Offset] : Offsets)
        {
            VINFO("Name = {}", Key);
            Names.push_back(Key);
        }

        return Names;
    }

    // ustar\20
    Byte USTAR[] = { 0x75, 0x73, 0x74, 0x61, 0x72, 0x20 };

    std::optional<TAR::Object> LoadTAR(Binary B)
    {
        Emulator::Shit(B);
        B.Seek(0);
        TAR::Object Ret;
        while(B.Tell() < B.Len())
        {
            auto Head = B.Read<TAR::Header>();

            if(memcmp(Head.Magic, USTAR, sizeof(USTAR)) != 0)
            {
                VERROR("Invalid TAR Magic");
                return std::nullopt;
            }
            else
            {
                VINFO("Head = {} {} {}", Head.Name, Head.FileType, Cthulhu::Utils::ParseInt(Head.Size).Get());
            }

            I32 Size = OctalToDecimal(Cthulhu::Utils::ParseInt(Head.Size).Get());
            Ret.Offsets[(char*)Head.Name] = B.Tell() - sizeof(TAR::Header);
            B.Seek(Size);
        }

        Ret.File = B;
        return Ret;
    }
}