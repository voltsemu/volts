#pragma once

#include "Alias.h"

namespace RPCS3X::SFO
{
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
        U32 DataLength;
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

    struct Error
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

        static Variant<SFO, Error> Load(const String& Path);
    };
}