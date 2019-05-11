#pragma once

#include <FileSystem/BufferedFile.h>
#include "PS3/Util/Endian.h"
#include "Core/Memory/Binary.h"

namespace Volts::PS3
{
    namespace FS = Cthulhu::FileSystem;
    
    enum class Width : Cthulhu::U8
    {
        B32,
        B64,
    };

    namespace ELF
    {
        using Binary = Cthulhu::Binary;
#if 0
        struct Section
        {
            Cthulhu::U64 Length;
            Cthulhu::Byte* Data;
        };

        struct Binary
        {
            Width BitWidth;
            Cthulhu::U64 Entry;

            Cthulhu::Array<Section> Sections;

            Cthulhu::U64 Length;
            Cthulhu::Byte* RawData;
        };
#endif
    }

    namespace UNSELF
    {
        // decrypt SELF file (EBOOT.BIN)
        Cthulhu::Option<ELF::Binary> DecryptSELF(FS::BufferedFile& File, Cthulhu::U8* Key = nullptr);
    }
}

