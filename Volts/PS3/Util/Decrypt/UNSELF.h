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
    }
}

