#include "SFO.h"

#include "Convert.h"

namespace RPCS3X::SFO
{
    // this serves as a redirector to a key, value pair stored in the file
    // we put it here instead of in the header as it is a purley internal data structure
    // not used anywhere outside this function 
    struct IndexTableEntry
    {
        // relative key offset from the asbolute key table offset
        U16 KeyOffset;

        // the format of the data type
        Format Format;

        // the actual length of the data
        U32 DataLength;

        // the maximum length of the data
        U32 MaxLength;

        //the offset of the data from the absolute data table offset
        U32 DataOffset;
    };

    // this is also internal
    // this represents the header of a file
    struct Header
    {
        // magic number for .SFO
        U32 Magic;

        // sfo version number
        U32 Version;

        // absolute offset of the key table
        U32 KeyOffset;

        // absolute offset of the data table
        U32 DataOffset;

        // the total number of entries
        U32 TotalEntries;
    };

    Result<SFO, Error> Load(FS::BufferedFile& File)
    {
        //make sure the file is valid so we dont segfault
        if(!File.Valid())
            return Fail<SFO, Error>(Error::BadFile);
        
        // read the first sizeof(Header) bytes into a Header
        // this contains some basic metadata about the file structure
        // as well as magic numbers and version numbers
        const auto Stats = File.Read<Header>();

        // 1179865088ULL is actually "\0PSF" reinterpreted to U32
        // thats the magic that is always at the top of .SFO files
        if(Stats.Magic != "\0PSF"_U32)
            return Fail<SFO, Error>(Error::BadMagic);

        // while other versions may technically be valid, we do this anyway just to be sure
        if(Stats.Version != 0x101)
            return Fail<SFO, Error>(Error::BadVersion);

        // create the object
        SFO Object;

        // for every entry in the SFO 
        for(U32 I = 0; I < Stats.TotalEntries; I++)
        {
            // read in the first redirector
            const auto Redirector = File.Read<IndexTableEntry>();

            // get the current depth in the file so we can get back
            const U32 Dist = File.CurrentDepth();

            // seek to the absolute key table offset plus the relative key offset from the redirector
            File.Seek(Stats.KeyOffset + Redirector.KeyOffset);

            // create a string to store the key
            String Key;
            
            // read in the key on char at a time
            while(C8 C = File.Next())
                Key += C;

            // then seek to the absolute offset of the data table plus the relative data offset from the redirector
            File.Seek(Stats.DataOffset + Redirector.DataOffset);

            // read in as many bytes as needed, 
            // we go with MaxLength instead of DataLength
            // so that we get null bytes in strings
            Array<U8> Bytes = File.ReadBytes(Redirector.MaxLength);

            // create a value from the data we collected
            Value Set = { Redirector.Format, Bytes };

            // add it to the table
            Object[Key] = Set;

            // now we use the value from erlier to seek back
            // to the byte after the last redirector entry
            File.Seek(Dist);
        }

        // we're done with the file we can close it
        File.Close();

        // and the parsing is done, we can return the object
        return Pass<SFO, Error>(Object);
    }
}