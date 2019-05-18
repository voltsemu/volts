#pragma once

#include <FileSystem/BufferedFile.h>
#include "PS3/Util/Endian.h"
#include "Core/Memory/Binary.h"

namespace Volts::PS3
{
    namespace FS = Cthulhu::FileSystem;

    namespace ELF
    {
        using Binary = Cthulhu::Binary;
    }

    namespace UNSELF
    {
        // decrypt SELF file (EBOOT.BIN)
        Cthulhu::Option<ELF::Binary> DecryptSELF(FS::BufferedFile& File, Cthulhu::U8* Key = nullptr);
    }
}

