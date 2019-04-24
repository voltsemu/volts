#pragma once

#include <FileSystem/BufferedFile.h>

namespace RPCS3X
{
    namespace FS = Cthulhu::FileSystem;
    using ELFBinary = Cthulhu::Array<Cthulhu::Byte>;
    ELFBinary DecryptSELF(FS::BufferedFile& File);
}

