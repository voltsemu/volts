#include "TAR.h"

#include "Core/Macros.h"

namespace Volts::TAR
{
    using namespace Cthulhu;

    struct Header
    {
        C8 Name[100];
        Pad Padding[24];
        Byte Size[12];
        Byte Time[12];
        Byte Checksum[8];
        U8 FileType;
        Byte Magic[6];
        Pad Padding2[82];
        Byte Prefix[155];
        Byte Padding3[12];
    };

    I32 OctToDec(I32 Oct)
    {
        I32 Dec = 0;
        U32 Rem;
        U32 I = 0;

        while(Oct != 0)
        {
            Rem = Oct % 10;
            Oct /= 10;
            Dec += Rem * Math::Pow(8U, I);
            I++;
        }

        return Dec;
    }

    Format::Format(FS::BufferedFile& F)
        : File(F)
    {
        
    }

    Array<String> Format::Filenames()
    {
        auto Head = File.Read<Header>();
    }

    Binary Format::GetFile(const String& Name)
    {
        return {};
    }

    bool Format::Parse()
    {
        return true;
    }
}