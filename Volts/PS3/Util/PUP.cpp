#include "PUP.h"
#include "Core/Memory/Memory.h"

namespace Volts::PS3::PUP
{
    using namespace Cthulhu;
    
    struct Header
    {
        Big<U64> Magic;
        Big<U64> PackageVersion;
        Big<U64> ImageVersion;
        Big<U64> FileCount;
        Big<U64> HeaderLength;
        Big<U64> FileLength;
    };

    static_assert(sizeof(Header) == 48);

    using HeaderHash = Byte[20];

    bool Format::Parse()
    {
        File.Seek(0);
        auto Head = File.Read<Header>();

        if(Head.Magic.Data != "SCEUF\0\0\0"_U64)
        {
            LOG_ERROR(FIRMWARE, "Invalid PUP magic");
            return false;
        }

        FileCount = Head.FileCount;

        // TODO: maybe use unique pointers or something
        Files = new FileTable[FileCount];
        Hashes = new HashTable[FileCount];

        File.ReadN(Files, FileCount * sizeof(FileTable));
        File.ReadN(Hashes, FileCount * sizeof(HashTable));

        return true;
    }

    Cthulhu::Binary Format::GetFile(U32 ID)
    {
        for(U32 I = 0; I < FileCount; I++)
        {
            if(Files[I].ID == ID)
            {
                Binary Bin;

                // TODO: cleanup, probalby modify cthulhu to do this
                File.Seek(Files[I].Offset);
                Byte* Buffer = new Byte[Files[I].Length];
                File.ReadN(Buffer, Files[I].Length);
                Bin.Write(Buffer, Files[I].Length);
                Bin.Seek(0);

                delete[] Buffer;

                return Bin;
            }
        }

        return {};
    }
}