#include "TAR.h"

#include "Core/Macros.h"

namespace Volts::TAR
{
    using namespace Cthulhu;

    Format::Format(FS::BufferedFile& File)
    {

    }

    Format::Format(Binary& B)
    {

    }

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

    Array<String> Format::Filenames()
    {
        return {};
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