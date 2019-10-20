#pragma once

#include <Core/Memory/Binary.h>
#include <FileSystem/BufferedFile.h>

namespace Volts::Utils
{
    namespace FS = Cthulhu::FileSystem;

    Cthulhu::Binary LoadSELF(FS::BufferedFile&& File);
}