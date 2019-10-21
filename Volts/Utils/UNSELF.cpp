#include "UNSELF.h"

#include "Core/Emulator.h"
#include "Core/Endian.h"
#include "Core/Convert.h"

#include <vector>

namespace Volts::Utils
{
    namespace SCE
    {
        struct Header
        {
            U32 Magic;
            Big<U32> Version;
            Big<U16> Type;
            Big<U16> Category;
            Big<U32> MetadataStart;
            Big<U64> MetadataEnd;
            Big<U64> Size;
        };

        static_assert(sizeof(Header) == 32);
    }

    namespace SELF
    {
        struct Header
        {
            Big<U64> Type;
            Big<U64> AInfoOffset;
            Big<U64> ELFOffset;
            Big<U64> PHeaderOffset;
            Big<U64> SHeaderOffset;
            Big<U64> SInfoOffset;
            Big<U64> VersionOffset;
            Big<U64> ControlOffset;
            Big<U64> ControlLength;
            Pad Padding[8];
        };

        static_assert(sizeof(Header) == 80);
    };

    namespace ELF
    {
        struct Header
        {
            U32 Magic;

            U8 Class;
            U8 Endian;
            U8 SVersion;
            U8 ABI;

            U64 ABIVersion;

            Big<U16> Type;
            Big<U16> Machine;
            Big<U32> Version;

            Big<U64> Entry;
            Big<U64> PHOffset;
            Big<U64> SHOffset;

            Big<U32> Flags;
            Big<U16> HeaderSize;

            Big<U16> PHEntrySize;
            Big<U16> PHCount;

            Big<U16> SHEntrySize;
            Big<U16> SHCount;

            Big<U16> StringIndex;
        };

        static_assert(sizeof(Header) == 64);

        struct SectionHeader
        {
            Big<U32> StringOffset;
            Big<U32> Type;

            Big<U64> Flags;
            Big<U64> VirtualAddress;
            Big<U64> Offset;
            Big<U64> Size;

            Big<U32> Link;
            Big<U32> Info;

            Big<U64> Align;
            Big<U64> EntrySize;
        };

        static_assert(sizeof(SectionHeader) == 64);

        struct ProgramHeader
        {
            Big<U32> Type;

            Big<U32> Flags;
            Big<U64> Offset;
            Big<U64> VirtualAddress;
            Big<U64> PhysicalAddress;
            Big<U64> FileSize;
            Big<U64> MemorySize;

            Big<U64> Align;
        };

        static_assert(sizeof(ProgramHeader) == 56);
    };

    struct AppInfo
    {
        Big<U64> AuthID;
        Big<U32> VendorID;
        Big<U32> Type;
        Big<U64> Version;
        Pad Padding[8];
    };

    static_assert(sizeof(AppInfo) == 32);

    struct Decryptor
    {
        FS::BufferedFile& File;

        Decryptor(FS::BufferedFile& F)
            : File(F)
        {}

    private:
        SCE::Header SCEHead;
        ELF::Header ELFHead;
        SELF::Header SELFHead;

        AppInfo Info;

        std::vector<ELF::SectionHeader> SHeaders;
        std::vector<ELF::ProgramHeader> PHeaders;

    public:

        bool ReadHeaders()
        {
            SCEHead = File.Read<SCE::Header>();

            if(SCEHead.Magic != "SCE\0"_U32)
            {
                VERROR("Invalid SCE magic");
                return false;
            }

            SELFHead = File.Read<SELF::Header>();
            Info = File.Read<AppInfo>();
            ELFHead = File.Read<ELF::Header>();

            return true;
        }

        bool ReadMetadata()
        {
            return false;
        }

        void Decrypt()
        {
            
        }

        Cthulhu::Binary ToELF()
        {
            return {};
        }
    };

    Cthulhu::Binary LoadSELF(FS::BufferedFile&& File)
    {
        Decryptor Dec = File;

        if(!Dec.ReadHeaders())
        {
            VERROR("Failed to read EBOOT headers");
            return {};
        }

        if(!Dec.ReadMetadata())
        {
            VERROR("Failed to read metadata");
            return {};
        }

        Dec.Decrypt();

        return Dec.ToELF();
    }   
}