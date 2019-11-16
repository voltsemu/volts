#pragma once

#include "PCH.h"

#include <Cthulhu/System/Binary.h>

#include "Core/Endian.h"
#include "Core/Macros.h"
#include "Core/Convert.h"

namespace Volts::Utils
{
    using namespace Cthulhu;

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

        template<typename T>
        struct Object
        {
            std::vector<Entry> Files;
            std::vector<Hash> Hashes;

            Interfaces::Stream<T> File;

            Memory::Binary GetFile(U64 ID);
        };
    }

    
    template<typename T>
    std::optional<PUP::Object<T>> LoadPUP(Interfaces::Stream<T>&& File)
    {
        File.Seek(0);

        auto Head = File.Read<PUP::Header>();

        if(Head.Magic != "SCEUF\0\0\0"_U64)
        {
            VERROR("PUP file had invalid magic");
            return std::nullopt;
        }

        PUP::Object<T> Ret = {};

        for(U32 I = 0; I < Head.FileCount; I++)
            Ret.Files.push_back(File.Read<PUP::Entry>());

        for(U32 I = 0; I < Head.FileCount; I++)
            Ret.Hashes.push_back(File.Read<PUP::Hash>());

        Ret.File = std::move(File);

        return Ret;
    }
}