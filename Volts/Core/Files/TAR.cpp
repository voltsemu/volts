#include "TAR.h"

#include "Core/Macros.h"
#include "Core/Logger/Logger.h"

#include <Core/Memory/Binary.h>

namespace Volts::TAR
{
    using namespace Cthulhu;

    Format::Format(Binary* B)
        : Bin(B)
    {
        InitialOffset = Bin->Depth();
    }

    PACKED_STRUCT(Header,
    {
        C8 Name[100];
        Pad Padding[24];
        Byte Size[12];
        Byte Time[12];
        Byte Checksum[8];
        U8 Link;
        Byte LinkName[100];
        Byte Magic[6];
        U16 Version;
        Byte OwnerName[32];
        Byte GroupName[32];
        U64 Major;
        U64 Minor;
        Byte Prefix[155];
        // this technically shouldnt be here, but it fixes some bugs so here it stays until
        // i can figure out how to fix it properly
        // TODO: fixme
        Pad Padding2[12];
    })

    int i = sizeof(Header);

    static_assert(sizeof(Header) == 512);

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

    constexpr Byte TARMagic[] = {'u', 's', 't', 'a', 'r', '\0'};

    Array<String> Format::Filenames()
    {
        U32 Depth = Bin->Depth();
        Array<String> Names;

        while(true) 
        {
            auto Head = Bin->Read<Header>();
            Names.Append(Head.Name);
            if(Memory::Compare<Byte>(Head.Magic, TARMagic, sizeof(TARMagic)) != 0)
            {
                Bin->Seek(Depth);
                break;
            }

            I32 Size = OctToDec(Utils::ParseInt(Head.Size));
            Bin->Seek(Bin->Depth() + Size);
        }

        Bin->Seek(Depth);
        return Names;
    }

    Binary Format::GetFile(const String& Name)
    {
        U32 Depth = Bin->Depth();
        auto Ret = FindFile(Name);
        Bin->Seek(Depth);

        return Ret;
    }

    Binary Format::FindFile(const String& Name)
    {
        auto Head = Bin->Read<Header>();

        if(Memory::Compare<Byte>(Head.Magic, TARMagic, sizeof(TARMagic)) != 0)
        {
            LOG_ERROR(TAR, "Invalid TAR Magic");
            return {};
        }
                
        I32 Size = OctToDec(Utils::ParseInt(Head.Size));

        C8* HeaderName = Head.Name;
        // TODO: modify the cthulhu functions to use C8 not char
        if(CString::Compare((char*)HeaderName, Name.CStr()) != 0)
        {
            Bin->Seek(Bin->Depth() + Size);
            return GetFile(Name);
        }

        Binary Out;

        Byte* Data = new Byte[Size+1];
        Bin->ReadN(Data, Size);
        Out.Write(Data, Size+1);
        delete[] Data;
        Out.Seek(0);

        return Out;
    }
}