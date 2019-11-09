#pragma once

#include <optional>
#include <map>

#include "Core/Macros.h"
#include "Core/Emulator.h"

#include <Core/Memory/Binary.h>

#include <FileSystem/BufferedFile.h>

namespace Volts::Utils
{
    using namespace Cthulhu;

    namespace FS = Cthulhu::FileSystem;

    namespace TAR
    {
        struct Header
        {
            C8 Name[100];
            C8 Mode[8];
            C8 UID[8];
            C8 GID[8];
            Byte Size[12];
            Byte MTime[12];
            Byte CheckSum[8];
            Byte FileType;
            C8 LinkName[100];
            // always "ustar\20"
            Byte Magic[6];
            U8 Version[2];
            C8 UName[32];
            C8 GName[32];
            C8 DevMajor[8];
            C8 DevMinor[8];
            C8 Prefix[155];
            Pad Padding[12];
        };

        static_assert(sizeof(Header) == 512);

        struct Object
        {
            Binary File;
            std::map<std::string, U64> Offsets;
        };
    }

    TAR::Object LoadTAR(Binary B);
}