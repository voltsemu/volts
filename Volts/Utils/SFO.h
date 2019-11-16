#pragma once

#include "PCH.h"

#include "Core/Endian.h"

namespace Volts::Utils
{
    using namespace Cthulhu;
    using namespace Types;
    
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

    struct IndexTableEntry
    {
        // relative key offset from the asbolute key table offset
        Little<U16> KeyOffset;

        // the format of the data type
        Little<Format> DataFormat;

        // the actual length of the data
        Little<U32> DataLength;

        // the maximum length of the data
        Little<U32> MaxLength;

        //the offset of the data from the absolute data table offset
        Little<U32> DataOffset;
    };

    // this represents the header of a file
    struct Header
    {
        // magic number for .SFO
        Little<U32> Magic;

        // sfo version number
        Little<U32> Version;

        // absolute offset of the key table
        Little<U32> KeyOffset;

        // absolute offset of the data table
        Little<U32> DataOffset;

        // the total number of entries
        Little<U32> TotalEntries;
    };

    template<typename T>
    std::map<std::string, SFOValue> LoadSFO(Interfaces::Stream<T>&& File)
    {
        const auto Stats = File.Read<Header>();

        // make sure the magic is valid
        if(Stats.Magic != "\0PSF"_U32)
        {
            VERROR("Invalid PARAM.SFO magic");
            return {};
        }

        // while other versions may technically be valid, we do this anyway just to be sure
        if(Stats.Version != 0x101)
        {
            VERROR("Invalid PARAM.SFO version");
            return {};
        }

        // create the object
        SFO Object;

        // for every entry in the SFO
        for(U32 I = 0; I < Stats.TotalEntries; I++)
        {
            // read in the first redirector
            const auto Redirector = File.Read<IndexTableEntry>();

            // get the current depth in the file so we can get back
            const U32 Dist = File.Tell();

            // seek to the absolute key table offset plus the relative key offset from the redirector
            File.Seek(Stats.KeyOffset + Redirector.KeyOffset);

            std::string Key;

            while(Char C = File.Read<Char>())
                Key += C;

            File.Seek(Stats.DataOffset + Redirector.DataOffset);

            std::vector<Byte> Bytes(Redirector.MaxLength);

            File.ReadN(Bytes.data(), Redirector.MaxLength);

            SFOValue Set = { Redirector.DataFormat, Bytes };

            Object[Key] = Set;

            File.Seek(Dist);
        }

        return Object;
    }
}