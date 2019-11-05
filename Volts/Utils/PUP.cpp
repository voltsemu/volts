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

    std::optional<PUP::Object> LoadPUP(FS::BufferedFile&& File)
    {
        PUP::Object Ret = File;

        auto Head = File.Read<Header>();
        if(Head.Magic != "SCEUF\0\0\0"_U64)
        {
            VERROR("PUP file had invalid magic {}", Head.Magic);
            return std::nullopt;
        }

        Ret.Files.reserve(Head.FileCount);
        Ret.Hashes.reserve(Head.FileCount);

        for(U32 I = 0; I < Head.FileCount; I++)
            Ret.Files.push_back(File.Read<PUP::Entry>());

        for(U32 I = 0; I < Head.FileCount; I++)
            Ret.Hashes.push_back(File.Read<PUP::Hash>());

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
                    File.Seek(Entry.Offset);
                    Binary Out;
                    Out.Reserve(Entry.Length);
                    File.ReadN(Out.GetData(), Entry.Length);

                    return Out;
                }
            }

            return {};
        }
    }
}