#pragma once

#include <FileSystem/BufferedFile.h>
#include "Core/Endian.h"
#include "Core/Memory/Binary.h"

namespace Volts::PS3
{
    namespace ELF
    {
        using Binary = Cthulhu::Binary;
    }

    namespace UNSELF
    {
        // decrypt SELF file (EBOOT.BIN)
        Cthulhu::Option<ELF::Binary> DecryptSELF(Cthulhu::FileSystem::BufferedFile& File, Cthulhu::Byte* Key = nullptr);

        // decrypt a firmware file and return an array of sections
        Cthulhu::Array<ELF::Binary> DecryptFirmware(Cthulhu::Binary& Bin, const Cthulhu::Byte ERK[32], const Cthulhu::Byte RIV[16]);
    }
}

