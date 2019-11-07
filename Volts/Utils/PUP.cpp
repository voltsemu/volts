#include "PUP.h"

#include "Core/Convert.h"
#include "Core/Emulator.h"

namespace Volts::Utils
{
    struct Header
    {
        U64 Magic;
        Big<U64> PackageVersion;
        Big<U64> ImageVersion;
        Big<U64> FileCount;
        Big<U64> HeaderLength;
        Big<U64> DataLength;
    };

    static_assert(sizeof(Header) == 48);

    std::optional<PUP::Object> LoadPUP(FS::BufferedFile* File)
    {
        if(!File->Valid())
        {
            VERROR("PUP file handle was invalid");
            return std::nullopt;
        }
        PUP::Object Ret = File;

        File->Seek(0);

        auto Head = File->Read<Header>();
        if(Head.Magic != "SCEUF\0\0\0"_U64)
        {
            VINFO("Header was {} {} {} {} {} {}", 
                Head.Magic,
                Head.PackageVersion.Get(),
                Head.ImageVersion.Get(),
                Head.FileCount.Get(),
                Head.HeaderLength.Get(),
                Head.DataLength.Get()
            );
            VERROR("PUP file had invalid magic");
            return std::nullopt;
        }

        for(U32 I = 0; I < Head.FileCount; I++)
            Ret.Files.push_back(File->Read<PUP::Entry>());

        for(U32 I = 0; I < Head.FileCount; I++)
            Ret.Hashes.push_back(File->Read<PUP::Hash>());

        return Ret;
    }

    namespace PUP
    {
        Binary Object::GetFile(U64 ID)
        {
            for(auto& Entry : Files)
            {
                if(Entry.ID == ID)
                {
                    File->Seek(Entry.Offset);
                    Binary Out;
                    Out.Reserve(Entry.Length);
                    File->ReadN(Out.GetData(), Entry.Length);

                    return Out;
                }
            }

            return {};
        }
    }
}