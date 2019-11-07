#include "PUP.h"

#include "Core/Convert.h"
#include "Core/Emulator.h"

namespace Volts::Utils
{
    namespace PUP
    {
        // the first 48 bytes of a PUp file is the header
        struct Header
        {
            // magic is always 302078313299 or "SCEUF\0\0\0"
            Little<U64> Magic;
            // ususally 1
            Big<U64> PackageVersion;
            // usually 66412
            Big<U64> ImageVersion;
            // the amount of files in the PUP object
            Big<U64> FileCount;
            // the size of the header + file table + hash table in bytes
            Big<U64> HeaderLength;
            // the length of the rest of the data
            Big<U64> DataLength;
        };

        static_assert(sizeof(Header) == 48);
    }

    std::optional<PUP::Object> LoadPUP(FS::BufferedFile* File)
    {
        if(!File->Valid())
        {
            VERROR("PUP file handle was invalid");
            return std::nullopt;
        }

        File->Seek(0);

        auto Head = File->Read<PUP::Header>();

        if(Head.Magic != "SCEUF\0\0\0"_U64)
        {
            VERROR("PUP file had invalid magic");
            return std::nullopt;
        }

        PUP::Object Ret{File};

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
                VINFO("Entry = {}", Entry.ID);
                if(Entry.ID == ID)
                {
                    File->Seek(Entry.Offset);
                    Binary Out;
                    Out.Reserve(Entry.Length);
                    VINFO("{} {} {}", Entry.Offset, Entry.Length, Out.RealLength());
                    File->ReadN(Out.GetData(), Entry.Length);
                    VINFO("{} {} {}", Out.Tell(), Out.Len(), Out.RealLength());
                    Out.Seek(Entry.Length);

                    return Out;
                }
            }

            return {};
        }
    }
}