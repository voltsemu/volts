#include "UNSELF.h"

#include "Core/Utilities/Endian.h"
#include "Core/Utilities/Convert.h"

using namespace Cthulhu;

namespace RPCS3X
{
    // SELF files are big, complicated files that were designed to not be easy to decrypt
    // this code is going to reflect the many stages required to bypass all of sonys security
    namespace SCE
    {
        struct Header
        {
            //sould always be 4539219ULL or "SCE\0"
            U32 Magic;
            Big<U32> HeaderVersion;
            Big<U16> KeyType;
            Big<U16> FileCategory;
            Big<U32> Metadataoffset;
            Big<U64> HeaderLength;
            Big<U64> DataLength;
        };
    }

    namespace SELF
    {
        struct Header
        {
            U64 Type;
            Big<U64> InfoOffset;
            Big<U64> ELFOffset;
            Big<U64> ProgramHeaderOffset;
            Big<U64> SectionHeaderOffset;
            Big<U64> SectionInfoOffset;
            Big<U64> VersionOffset;
            Big<U64> ControlOffset;
            Big<U64> ControlLength;
            U8 Padding[8];
        };
    }

    bool Is32BitSELF(FS::BufferedFile& File)
    {
        File.Seek(sizeof(SCE::Header));
        auto Header = File.Read<SELF::Header>();

        File.Seek(Header.ELFOffset);
        
        // do this instead of ReadBytes<T> to speed up the program
        U8* Class = (U8*)File.Read<U64>();
        return Class[4] == 1;
    }

    // based off of stuff found in https://www.psdevwiki.com/ps3/SELF_File_Format_and_Decryption#Extracting_an_ELF
    ELFBinary DecryptSELF(FS::BufferedFile& File)
    {
        // Read in the SCE header
        auto SCE = File.Read<SCE::Header>();

        if(SCE.Magic != "SCE\0"_U32)
            return Array<Byte>();
        return Array<Byte>();
    }
}

