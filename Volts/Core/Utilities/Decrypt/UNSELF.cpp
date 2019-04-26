#include "UNSELF.h"

#include "Core/Utilities/Endian.h"
#include "Core/Utilities/Convert.h"

using namespace Cthulhu;

namespace Volts
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
            Big<U32> MetadataOffset;
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

    namespace ELF
    {
        struct SmallHeader
        {
            Big<U32> Magic;
            U8 Class;
            U8 Endianness;
            U8 Version;
            U8 ABI;
            U8 ABIVersion;
            U8 Padding[7];
        };

        template<typename T>
        struct BigHeader
        {
            Big<U16> Type;
            Big<U16> Machine;
            Big<U32> Version;
            
            Big<T> Entry;
            Big<T> PHOffset;
            Big<T> SHOffset;
            
            Big<U32> Flags;
            Big<U16> HeaderSize;
            
            Big<U16> PHEntrySize;
            Big<U16> PHCount;
            
            Big<U16> SHEntrySize;
            Big<U16> SHCount;
        };

        template<typename T>
        struct ProgramHeader {};

        template<> struct ProgramHeader<U32> 
        {
            Big<U32> Type;
            Big<U32> Flags;

            Big<U32> Offset;
            Big<U32> VirtualAddress;
            Big<U32> PhysicalAddress;
            Big<U32> FileSize;
            Big<U32> MemorySize;

            Big<U32> Align;
        };

        template<> struct ProgramHeader<U64> 
        {
            Big<U32> Type;

            Big<U32> Offset;
            Big<U32> VirtualAddress;
            Big<U32> PhysicalAddress;
            Big<U32> FileSize;
            Big<U32> MemorySize;
            
            Big<U32> Flags;
            Big<U32> Align;
        };

        template<typename T>
        struct SectionHeader
        {
            Big<U32> NameOffset;
            Big<U32> Type;

            Big<T> Flags;
            Big<T> VirtualAddress;
            Big<T> Offset;
            Big<T> Size;

            Big<U32> Link;
            Big<U32> Info;

            Big<T> Align;
            Big<T> EntrySize;
        };
    }

    struct AppInfo
    {
        Big<U64> AuthID;
        Big<U32> VendorID;
        Big<U32> Type;
        Big<U32> Version;
        U8 Padding[12];
    };

    struct Decryptor
    {
        Decryptor(FS::BufferedFile& F)
            : File(F)
        {}

    public:

        bool LoadSCE()
        {
            // Read in the SCE header
            // this is essentially metadata the ps3 uses internally
            // we need some, but not all of it
            auto SCE = File.Read<SCE::Header>();

            printf(
                "SCE::Header\n"
                "Magic = %u\n"
                "HeaderVersion = %u\n"
                "KeyType = %u\n"
                "FileCategory = %u\n"
                "MetadataOffset = %u\n"
                "HeaderLength = %llu\n"
                "DataLength = %llu\n\n",
                SCE.Magic,
                SCE.HeaderVersion.Get(),
                SCE.KeyType.Get(),
                SCE.FileCategory.Get(),
                SCE.MetadataOffset.Get(),
                SCE.HeaderLength.Get(),
                SCE.DataLength.Get()
            );

            if(SCE.Magic != "SCE\0"_U32)
            {
                return false;
            }

            return true;
        }

        bool LoadSELF()
        {
            SELFHead = File.Read<SELF::Header>();
            
            printf(
                "SELF::Header\n"
                "Type = %llu\n"
                "AppInfoOffset = %llu\n"
                "ELFOffset = %llu\n"
                "PHeadOffset = %llu\n"
                "SHeadOffset = %llu\n"
                "SInfoOffset = %llu\n"
                "VersionOffset = %llu\n"
                "ControlInfoOffset = %llu\n"
                "ControlLength = %llu\n\n",
                SELFHead.Type,
                SELFHead.InfoOffset.Get(),
                SELFHead.ELFOffset.Get(),
                SELFHead.ProgramHeaderOffset.Get(),
                SELFHead.SectionHeaderOffset.Get(),
                SELFHead.SectionInfoOffset.Get(),
                SELFHead.VersionOffset.Get(),
                SELFHead.ControlOffset.Get(),
                SELFHead.ControlLength.Get()
            );

            return true;
        }

        bool LoadAppInfo()
        {
            File.Seek(SELFHead.InfoOffset);
            Info = File.Read<AppInfo>();

            printf(
                "AppInfo\n"
                "AuthID = %llu\n"
                "VendorID = %u\n"
                "Type = %u\n"
                "Version = %u\n\n",
                Info.AuthID.Get(),
                Info.VendorID.Get(),
                Info.Type.Get(),
                Info.Version.Get()
            );

            return true;
        }

        bool LoadELF()
        {
            File.Seek(SELFHead.ELFOffset);

            auto Small = File.Read<ELF::SmallHeader>();

            printf(
                "ELF::SmallHeader\n"
                "Magic = %u\n"
                "Class = %u\n"
                "Endian = %u\n"
                "Version = %u\n"
                "ABI = %u\n"
                "ABIVersion = %u\n\n",
                Small.Magic.Get(),
                Small.Class,
                Small.Endianness,
                Small.Version,
                Small.ABI,
                Small.ABIVersion
            );

            if(Small.Magic != 0x7F454C46)
            {
                return false;
            }

            if(Small.Class == 1)
            {
                // is 32 bit
                ELFHead32 = File.Read<ELF::BigHeader<U32>>();
            }
            else
            {
                // must be 64 bit otherwise
                ELFHead64 = File.Read<ELF::BigHeader<U64>>();
            }

            return true;
        }

        bool LoadHeaders() 
        {
            // because of short curcuiting && this actually works
            return LoadSCE() && LoadSELF() && LoadAppInfo() && LoadELF();
        }

        bool LoadData() 
        {
            return true;
        }

        ELF::Binary DecryptData() 
        { 
            return {}; 
        }

        FS::BufferedFile& File;

        SCE::Header SCEHead;
        SELF::Header SELFHead;
        AppInfo Info;

        union
        {
            ELF::BigHeader<U32> ELFHead32;
            ELF::BigHeader<U64> ELFHead64;
        };

        union
        {

        };
    };

    namespace UNSELF
    {
        // file format reference from https://www.psdevwiki.com/ps3/SELF_File_Format_and_Decryption#Extracting_an_ELF
        ELF::Binary DecryptSELF(FS::BufferedFile& File)
        {
            Decryptor Decrypt(File);

            if(!Decrypt.LoadHeaders())
            {
                return {};
            }

            if(!Decrypt.LoadData())
            {
                return {};
            }

            return Decrypt.DecryptData();
        }
    }
}

