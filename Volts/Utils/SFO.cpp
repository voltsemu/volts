#include "SFO.h"



namespace Volts::Utils
{
    using namespace Cthulhu;

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

    std::map<std::string, SFOValue> LoadSFO(FS::BufferedFile& Stream)
    {
        return {};
    }
}