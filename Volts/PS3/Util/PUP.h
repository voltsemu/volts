#pragma once

#include <FileSystem/BufferedFile.h>
#include <Core/Memory/Binary.h>

#include "Core/Endian.h"
#include "Core/Convert.h"
#include "Core/Macros.h"
#include "Core/Logger.h"

namespace Volts::PS3::PUP
{
    namespace FS = Cthulhu::FileSystem;

    struct FileTable
    {
        Big<Cthulhu::U64> ID;
        Big<Cthulhu::U64> Offset;
        Big<Cthulhu::U64> Length;
        Pad Padding[8];
    };

    static_assert(sizeof(FileTable) == 32);

    struct HashTable
    {
        Big<Cthulhu::U64> ID;
        Cthulhu::Byte Hash[20];
        Pad Padding[4];
    };

    static_assert(sizeof(HashTable) == 32);

    constexpr Cthulhu::U32 UpdateFiles = 768;

    struct Format
    {
        Format(FS::BufferedFile* F)
        {
            File.Claim(F);
        }

        ~Format()
        {
            delete[] Files;
            delete[] Hashes;
        }

        Cthulhu::Binary GetFile(Cthulhu::U32 ID);

        bool Parse();
    private:
        FS::BufferedFile File;
        FileTable* Files;
        HashTable* Hashes;
        Cthulhu::U32 FileCount;
    };
}