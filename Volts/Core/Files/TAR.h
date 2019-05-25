#pragma once

#include <FileSystem/BufferedFile.h>
#include "Core/Memory/Binary.h"

namespace Volts::TAR
{
    namespace FS = Cthulhu::FileSystem;

    struct Format
    {
        Format(FS::BufferedFile F)
            : File(F)
        {}

        Cthulhu::Array<Cthulhu::String> Filenames();

        Cthulhu::Binary GetFile(Cthulhu::String& Name);
    private:
        FS::BufferedFile File;
    };
}