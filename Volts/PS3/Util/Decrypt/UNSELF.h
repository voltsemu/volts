#pragma once

#include <FileSystem/BufferedFile.h>

namespace Volts::PS3
{
    namespace FS = Cthulhu::FileSystem;
    
    namespace ELF
    {
        using Binary = Cthulhu::Array<Cthulhu::Byte>;
    }

    namespace UNSELF
    {
        // decrypt SELF file (EBOOT.BIN)
        Cthulhu::Option<ELF::Binary> DecryptSELF(FS::BufferedFile& File, Cthulhu::U8* Key = nullptr);
    }
}

