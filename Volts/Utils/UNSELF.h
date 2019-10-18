#pragma once

#include <Core/Memory/Binary.h>
#include <FileSystem/BufferedFile.h>

namespace Volts::Utils
{
    namespace FS = Cthulhu::FileSystem;
    using ELF = Cthulhu::Binary;

    ELF LoadSELF(FS::BufferedFile&& File);
}