#pragma once

#include <Meta/Aliases.h>
#include <Core/Collections/CthulhuString.h>
#include <Core/Collections/Map.h>
#include <Core/Collections/Result.h>

#include <FileSystem/BufferedFile.h>

namespace Volts::PS3::SFO
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