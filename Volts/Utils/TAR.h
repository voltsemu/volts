#pragma once

#include <optional>
#include <map>

#include "Core/Macros.h"
#include "Core/Emulator.h"

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
            Pad Padding[24];
            Byte Size[12];
            Byte MTime[12];
            Byte CheckSum[8];
            Byte FileType;
            C8 LinkName[100];
            Byte Magic[6];
            Pad Padding2[82];
            C8 Prefix[155];
            Pad Padding3[12];
        };

        struct Object
        {
            FS::BufferedFile File;
            std::map<std::string, U64> Offsets;
        };
    }
}