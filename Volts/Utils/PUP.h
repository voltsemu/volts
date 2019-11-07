#pragma once

#include <FileSystem/BufferedFile.h>
#include <Core/Memory/Binary.h>

#include "Core/Endian.h"
#include "Core/Macros.h"

#include <optional>
#include <vector>

namespace Volts::Utils
{
    using namespace Cthulhu;

    namespace FS = Cthulhu::FileSystem;

    namespace PUP
    {
        struct Entry
        {
            Big<U64> ID;
            Big<U64> Offset;
            Big<U64> Length;
            Pad Padding[8];
        };

        struct Hash
        {
            Big<U64> ID;
            Byte Hash[20];
            Pad Padding[4];
        };

        struct Object
        {
            std::vector<Entry> Files;
            std::vector<Hash> Hashes;

            FS::BufferedFile* File;

            Object(FS::BufferedFile* F)
                : File(F)
            {}

            Binary GetFile(U64 ID);
        };
    }

    std::optional<PUP::Object> LoadPUP(FS::BufferedFile* File);
}