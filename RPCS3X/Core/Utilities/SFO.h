#pragma once

#include <Cthulhu/Meta/Aliases.h>
#include <Cthulhu/Core/Collections/CthulhuString.h>
#include <Cthulhu/Core/Collections/Map.h>
#include <Cthulhu/Core/Collections/Result.h>

#include <Cthulhu/FileSystem/BufferedFile.h>

namespace RPCS3X::SFO
{
    using namespace Cthulhu;
    namespace FS = FileSystem;

    enum class Format : U16
    {
        String = 516,
        Array = 4,
        Integer = 1024
    };

    struct Value
    {
        Format Type;
        Array<U8> Bytes;
    };

    enum class Error : U8
    {
        Ok,
        BadFile,
        BadMagic,
        BadVersion,
        InvalidKeyOffset,
        InvalidDataOffset,
    };

    using SFO = Map<String, Value>;

    Result<SFO, Error> Load(FS::BufferedFile& File);
}