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
        /**
         * @brief decrypt a SELF binary (EBOOT.BIN)
         * 
         * @param File the file to decrypt
         * @param Key an optional decryption key for NPDRM
         * @return Cthulhu::Option<ELF::Binary> Some if decryption worked otherwise None
         */
        Cthulhu::Option<ELF::Binary> DecryptSELF(Cthulhu::FileSystem::BufferedFile& File, Cthulhu::Byte* Key = nullptr);
    }
}

