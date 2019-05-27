#include "TAR.h"

#include "Core/Macros.h"
#include "Core/Logger/Logger.h"

namespace Volts::TAR
{
    using namespace Cthulhu;

    Format::Format(Binary& B)
        : Bin(B)
    {}

    struct Header
    {
        C8 Name[100];
        U64 Mode;
        U64 OwnerID;
        U64 GroupID;
        Byte Size[12];
        Byte Time[12];
        Byte Checksum[8];
        U8 Link;
        U8 Type;
        Byte LinkName[100];
        Byte Magic[6];
        U16 Version;
        Byte OwnerName[32];
        Byte GroupName[32];
        U64 Major;
        U64 Minor;
        Byte Prefix[155];
    };

    static_assert(sizeof(Header) == 512);

    Array<String> Format::Filenames()
    {
        return {};
    }

    constexpr Byte TARMagic[] = {'u', 's', 't', 'a', 'r', '\0'};

    Binary Format::GetFile(const String& Name)
    {
        auto Head = Bin.Read<Header>();
        LOGF_DEBUG(TAR, "Magic = %u %u %u %u %u %u", Head.Magic[0], Head.Magic[1], Head.Magic[2], Head.Magic[3], Head.Magic[4], Head.Magic[5]);
        //while(Memory::Compare<Byte>(Head.Magic, TARMagic, 6) != 0)
        {
            
        }

        return {};
    }

    bool Format::Parse()
    {
        return true;
    }
}