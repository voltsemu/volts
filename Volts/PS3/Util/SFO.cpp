#include "SFO.h"

#include "Convert.h"
#include "Endian.h"

namespace Volts::PS3::SFO
{
    using namespace Cthulhu;
    
    // unlike the rest of the ps3s data structures this data is stored in little endian
    
    // this serves as a redirector to a (key, value) pair stored in the file
    // we put it here instead of in the header as it is a purley internal data structure
    // not used anywhere outside this function 
    struct IndexTableEntry
    {
        // relative key offset from the asbolute key table offset
        Little<U16> KeyOffset;

        // the format of the data type
        Little<Format> Format;

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

    Option<SFO> Load(FS::BufferedFile& File)
    {
        //make sure the file is valid so we dont segfault
        if(!File.Valid())
            return None<SFO>();
        
        // read the first sizeof(Header) bytes into a Header
        // this contains some basic metadata about the file structure
        // as well as magic numbers and version numbers
        const auto Stats = File.Read<Header>();

        // make sure the magic is valid
        if(Stats.Magic != "\0PSF"_U32)
            return None<SFO>();

        // while other versions may technically be valid, we do this anyway just to be sure
        if(Stats.Version != 0x101)
            return None<SFO>();

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

        // once the parsing is finished we can return the data
        return Some(Object);
    }
}