#pragma once

#include <Meta/Aliases.h>
#include <Core/Collections/CthulhuString.h>
#include <Core/Collections/Map.h>
#include <Core/Collections/Result.h>

namespace RPCS3X::SFO
{
    using namespace Cthulhu;

    struct Header
    {
        U32 Magic;
        U32 Version;
        U32 KeyOffset;
        U32 DataOffset;
        U32 TotalEntries;
    };

    enum class Format : U16
    {
        String = 516,
        Array = 4,
        Integer = 1024
    };

    struct IndexTableEntry
    {
        U16 KeyOffset;
        Format DataFormat;

        U32 DataLength;
        U32 MaxLength;
        U32 DataOffset;
    };

    struct Value
    {
        Format Type;

        union
        {
            U64 Num;
            String* Str;
        };

        ~Value();
    };

    enum class Error : U8
    {
        Ok,
        BadMagic,
        InvalidKeyOffset,
        InvalidDataOffset,
    };

    struct SFO
    {
        Header Info;
        Map<String, Value> Data;

        static Result<SFO, Error> Load(const String& Path);
    };
}