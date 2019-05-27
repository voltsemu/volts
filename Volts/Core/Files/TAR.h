#pragma once

#include <FileSystem/BufferedFile.h>
#include <Core/Collections/Map.h>
#include "Core/Memory/Binary.h"

namespace Volts::TAR
{
    namespace FS = Cthulhu::FileSystem;

    struct Format
    {
        Format(Cthulhu::Binary* B);

        Cthulhu::Array<Cthulhu::String> Filenames();

        Cthulhu::Binary GetFile(const Cthulhu::String& Name);

        bool Parse();
    private:
        Cthulhu::Binary* Bin;
    };
}