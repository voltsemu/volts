#pragma once

#include <map>
#include <string>
#include <vector>

#include <FileSystem/BufferedFile.h>

#include "Core/Endian.h"

namespace Volts::Utils
{
    namespace FS = Cthulhu::FileSystem;

    enum class Format : U16
    {
        String = 516,
        Array = 4,
        Integer = 1028,
    };

    struct SFOValue
    {
        Format Type;

        std::vector<Byte> Data;
    };

    using SFO = std::map<std::string, SFOValue>;

    SFO LoadSFO(FS::BufferedFile&& File);
}