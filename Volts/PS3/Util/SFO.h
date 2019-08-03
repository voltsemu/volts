#pragma once

#include <Meta/Aliases.h>
#include <Core/Collections/CthulhuString.h>
#include <Core/Collections/Map.h>
#include <Core/Collections/Option.h>

#include <FileSystem/BufferedFile.h>

namespace Volts::PS3::SFO
{
    namespace FS = Cthulhu::FileSystem;

    // the format of the data stored in Bytes
    enum class Format : Cthulhu::U16
    {
        // a null terminated string of N characters
        String = 516,
        // an array of raw bytes, could also be a non null terminated string
        Array = 4,
        // always 4 bytes long, contains an integer, 
        // sometimes not all bytes of the value are used
        Integer = 1024
    };

    // an SFO value structure
    struct Value
    { 
        // what kind of data is being stored
        Format Type;

        // the data being stored
        Cthulhu::Array<Cthulhu::U8> Bytes;
    };

    // Represents a System File Object
    using SFO = Cthulhu::Map<
        // The name of the field
        Cthulhu::String, 
        // the value stored in the field
        Value
    >;

    // Actually load a system file object from a file
    Cthulhu::Option<SFO> Load(FS::BufferedFile& File);
}