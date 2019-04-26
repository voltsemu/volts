#pragma once

#include <FileSystem/BufferedFile.h>

namespace Volts
{
    namespace FS = Cthulhu::FileSystem;
    using namespace Cthulhu;
    
    namespace ELF
    {
        struct Binary
        {
            bool Valid = true;
            Array<Byte> Data;
        };
    }

    namespace UNSELF
    {
        ELF::Binary DecryptSELF(FS::BufferedFile& File);
    }
}

