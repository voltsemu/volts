#pragma once

#include "Utilities/Endian.h"

#include <Core/Traits/IsSame.h>

#include <FileSystem/BufferedFile.h>

namespace RPCS3X
{
    // the usual aliases
    using namespace Cthulhu;
    namespace FS = FileSystem;

    // ExecutableLinkFormat (ELF) data types
    namespace ELF
    {
        // Justification:
        //  - Q: Why use Big<T> instead of what rpcs3 does with Read8, Read16, Read32 and Read64?
        //  - A: each call to ReadN involves one FileIO operation and one byteswap instruction no matter what.
        //       This is very slow in comparison to a single File.Read<T>() call and byteswapping when needed.
        //       As well as that using a single fileIO operation reduces boilerplate code and allows these to 
        //       remain as POD types.
        //
        //  - Q: Whats with the templates on each type?
        //  - A: in rpcs3 each ELF types has 2 different types, 
        //       for example ELF::Header has Elf64_EHdr and Elf32_Ehdr.
        //       this has resulted in the buildup of excessive boilerplate code which leads to code bloat.
        //       I have decided that in modern C++17 templates and constraints are the correct way to
        //       go about differnt sizes.


        template<typename T>
        struct Header
        {
            // always remember the constraints
            static_assert(Same<T, U32>::Value || Same<T, U64>::Value);

            // magic file format number
            U32 Magic;
            U8 Class;
            U8 Data;
            U8 Curver;
            U8 ABI;
            Big<U64> ABIVersion;
            Big<U16> Type;
            Big<U16> Machine;
            Big<U32> Version;
            Big<T> Entry;
            Big<T> HeaderOffset;
            Big<T> SectionOffset;
            Big<U32> Flags;
            Big<U16> ELFHeaderSize;
            Big<U16> ProgramHeaderSize;
            Big<U16> ProgramEntries;
            Big<U16> SectionHeaderSize;
            Big<U16> SectionEntries;
            Big<U16> Index;
        };

        template<typename T>
        struct SectionHeader
        {
            // constraints
            static_assert(Same<T, U32>::Value || Same<T, U64>::Value);

            U32 Name;
            Big<U32> Type;
            Big<T> Flags;
            Big<T> Address;
            Big<T> Offset;
            Big<T> Size;
            Big<U32> Link;
            Big<U32> Info;
            Big<T> Align;
            Big<T> EntrySize;
        };

        template<typename T>
        struct ProgramHeader
        {
            // constraints
            static_assert(Same<T, U32>::Value || Same<T, U64>::Value);

            U32 Type;
            Big<U32> Offset;
            Big<T> ProgramHeaderOffset;
            Big<T> SectionHeaderOffset;
            Big<T> FileSize;
            Big<T> MemorySize;
            Big<T> Align;
        };
    }

    namespace SCE
    {
        struct Header
        {
            // should always be "\0SCE"
            U32 Magic;
            Big<U32> Version;
            Big<U16> Flags;
            Big<U16> Type;
            Big<U32> Offset;
            Big<U64> HeaderSize;
            Big<U64> DataSize;

            bool Valid() const { return Magic == 0x53434500; }
        };
    }

    namespace SELF
    {
        struct Header
        {
            Big<U64> Type;
            Big<U64> InfoOffset;
            Big<U64> ELFOffset;
            Big<U64> PHeaderOffset;
            Big<U64> SHeaderOffset;
            Big<U64> SInfoOffset;
            Big<U64> VersionOffset;
            Big<U64> ControlOffset;
            Big<U64> ControlSize;
            U8 Padding[4];
        };
    }

    // A list of the ways decrypting and parsing a self can fail
    enum class Error : U8
    {
        Ok,
        Decrypt,
        BadSCEHeader,
        BadSELFHeader,
        BadELFHeader
    };

    struct AppInfo
    {
        Big<U64> AuthID;
        Big<U32> VendorID;
        Big<U32> Type;
        Big<U32> Version;
        U8 Padding[12];
    };

    struct SELFBinary
    {
        SELFBinary(FS::BufferedFile File);

        Error Load();

        AppInfo Info;

    private:
        FS::BufferedFile File;
    };
}