#include "UNSELF.h"

#include "Core/Logger/Logger.h"
#include "Core/Macros.h"

#include "Core/Convert.h"
#include "Keys.h"
#include "AES/aes.h"

#include <zlib.h>


// SELF files have massive, somewhat overly complex headers that need parsing
// SELFs are usually called EBOOT.bin and contain the entire game executable
// this table is the header layout byte by byte as i understand it
// this is based on the ps3 dev wiki, rpcs3's self parser and the parser
// made by fail0verflow before sony forced them to take the source down
//
// Bytes 0..32 SCE::Header
//  0..4   Magic 
//      - always "SCE\0" or 4539219ULL
//
//  5..8   HeaderVersion 
//      - 2 = PS3 game
//      - 3 = PSVita game
//
//  9..10  KeyType
//      - we only care about this if its 0x8000
//        if its 0x8000 the self file is a debug file
//        if its a debug file nothing is encrypted
//
//  11..12 FileCategory
//      - 1 = SELF file, signed-elf. we need to decrypt this
//      - 2 = signed-revoke-list used by the ps3 hypervisor, we dont care about this
//      - 3 = signed-package are firmware packages used to update the ps3, we dont care about these
//      - 4 = secutiry-policy-profile used for internal ps3 files, 
//            we dont really need to care about this
//
//  13..16 MetadataStart
//      - the offset of the start of the metadata headers
//        this offset is relative to the end of the SCE::Header end byte
//        so the real offset is MetaDataStart + sizeof(SCE::Header)
//
//  17..24 MetadataEnd
//      - the offset of the last byte of the metadata headers
//        this can be used to calculate the length of the headers
//        with Size = MetadataEnd - sizeof(SCE::Header) - MetadataStart - sizeof(MetaData::Info)
//
//  25..32 DataLength
//      - This is the length of the data inside the file
//
// Bytes 33..112
//  33..40 Type
//      - Should always be 3
//
//  41..48 InfoOffset
//      - the absolute offset of the AppInfo struct
//
//  49..56 ELFOffset
//      - the absolute offset of the ELF header
//
// TODO the rest of these fields
//


namespace Volts::PS3
{
    using namespace Cthulhu;

    using Pad = Byte;

    namespace SCE
    {
        struct Header
        {
            U32 Magic;
            Big<U32> Version;
            Big<U16> Type; //
            Big<U16> Category;
            Big<U32> MetadataStart; //
            Big<U64> MetadataEnd; //
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

        struct ControlInfo
        {
            U32 Type;
            U32 Size;
            U64 Next;
            union
            {
                struct
                {
                    Big<U32> ControlFlag;
                    Pad Padding[28];
                } ControlFlags;

                static_assert(sizeof(ControlFlags) == 32);

                struct 
                {
                    U8 Constant[20];
                    U8 ELFDigest[20];
                    Big<U64> RequiredSystemVersion;
                } ELFDigest40;

                static_assert(sizeof(ELFDigest40) == 48);

                struct
                {
                    U8 ConstOrDigest[20];
                    Pad Padding[12];
                } ELFDigest30;

                static_assert(sizeof(ELFDigest30) == 32);

                struct
                {
                    U32 Magic; // always "NPD\0"
                    Big<U32> Version;
                    Big<U32> DRMType;
                    Big<U32> AppType;
                    Byte ContentID[48];
                    Byte Digest[16];
                    Byte INVDigest[16];
                    Byte XORDigest[16];
                    Pad Padding[16];
                } NPDRMInfo;

                static_assert(sizeof(NPDRMInfo) == 128);
            };
        };
    }

    struct AppInfo
    {
        Big<U64> AuthID;
        Big<U32> VendorID;
        Big<U32> Type;
        Big<U64> Version;
        Pad Padding[8];
    };

    static_assert(sizeof(AppInfo) == 32);

    namespace ELF
    {
        struct SmallHeader
        {
            U32 Magic;
            U8 Class;
            U8 Endian;
            U8 Version;
            U8 ABI;
            U8 ABIVersion;
            Pad Padding[7];
        };

        static_assert(sizeof(SmallHeader) == 16);

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

            Big<U16> StringIndex;
        };

        static_assert(sizeof(BigHeader<U32>) == 52 - sizeof(SmallHeader));
        static_assert(sizeof(BigHeader<U64>) == 64 - sizeof(SmallHeader));
    
        template<typename T>
        struct SectionHeader 
        {
            Big<U32> StringOffset;
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

        static_assert(sizeof(SectionHeader<U32>) == 40);
        static_assert(sizeof(SectionHeader<U64>) == 64);

        template<typename T>
        struct ProgramHeader {};

        template<>
        PACKED_STRUCT(ProgramHeader<U32>, {
            Big<U32> Type;

            Big<U32> Offset;
            Big<U32> VirtualAddress;
            Big<U32> PhysicalAddress;
            Big<U32> FileSize;
            Big<U32> MemorySize;
            Big<U32> Flags;

            Big<U32> Align;
        });

        static_assert(sizeof(ProgramHeader<U32>) == 32);

        template<>
        PACKED_STRUCT(ProgramHeader<U64>, {
            Big<U32> Type;
            
            Big<U32> Flags;
            Big<U64> Offset;
            Big<U64> VirtualAddress;
            Big<U64> PhysicalAddress;
            Big<U64> FileSize;
            Big<U64> MemorySize;

            Big<U64> Align;
        });

        static_assert(sizeof(ProgramHeader<U64>) == 56);
    }

    namespace MetaData
    {
        struct Info
        {
            Byte Key[16];
            Pad KeyPad[16];
            
            Byte IV[16];
            Pad IVPad[16];
        };

        static_assert(sizeof(Info) == 64);

        struct Header
        {
            Big<U64> SignatureLength;
            Big<U32> AlgorithmType; // always 1 (ECSDA)
            Big<U32> SectionCount;
            Big<U32> KeyCount;
            Big<U32> HeaderSize;
            Pad Padding[8];
        };

        static_assert(sizeof(Header) == 32);

        struct Section
        {
            Big<U64> Offset;
            Big<U64> Size;
            Big<U32> Type; // 1 = SectionHeader, 2 = ProgramHeader, 3 = SCEVersion
            Big<U32> Index;
            Big<U32> HashAlgo; // 2 = SHA1_HMAC, 3 = SHA1
            Big<U32> HashIndex;
            Big<U32> Encrypted; // 3 = yes, 1 = no
            
            // these may be Limits<U32>::Max()
            // but only when Encrypted != 3
            // so big numbers in here are not something to worry about
            Big<U32> KeyIndex; 
            Big<U32> IVIndex;

            Big<U32> Compressed; // 2 = yes, 1 = no
        };

        static_assert(sizeof(Section) == 48);
    }

    class SELFDecryptor
    {
        // file stream to read from
        FileSystem::BufferedFile& File;
    public:
        SELFDecryptor(FileSystem::BufferedFile& F)
            : File(F)
        {}

        ~SELFDecryptor()
        {

        }

    private:
        // if the structure is not relevant to generating the ELF file
        // at the end we dont need to store the whole struct
        // so we just store the variables we need to save space and to reduce the amount of variables 
        // flying around

        // SCE::Header
        U16 SCEFlags;
        U32 MetadataStart;
        U32 MetadataEnd;

        // AppInfo
        U32 SELFType;
        U64 SELFVersion;

        Array<SELF::ControlInfo> Controls;
        Array<MetaData::Section> MetaSections;
        // Other
        bool Arch64;
        Byte* Headers;
        U32 DataBufferLength = 0;


        // stuff that needs to be serialized into an ELF file

        // ELF header
        ELF::SmallHeader SmallELF;
        union
        {
            ELF::BigHeader<U32> BigELF32;
            ELF::BigHeader<U64> BigELF64;
        };

        union
        {
            ELF::SectionHeader<U32>* Section32Headers;
            ELF::SectionHeader<U64>* Section64Headers;
        };

        union
        {
            ELF::ProgramHeader<U32>* Program32Headers;
            ELF::ProgramHeader<U64>* Program64Headers;
        };

        SELF::ControlInfo ReadControlInfo()
        {
            static U32 I = 0;
            SELF::ControlInfo Info;

            Info.Type = File.Read<Big<U32>>();
            Info.Size = File.Read<Big<U32>>();
            Info.Next = File.Read<Big<U64>>();

            LOGF_DEBUG(SELF::ControlInfo, "%u -------------------", I++);
            LOGF_DEBUG(SELF::ControlInfo, "Type = %u", Info.Type);
            LOGF_DEBUG(SELF::ControlInfo, "Size = %u", Info.Size);
            LOGF_DEBUG(SELF::ControlInfo, "Next = %llu", Info.Next);

            if(Info.Type == 1)
            {
                Info.ControlFlags = File.Read<decltype(SELF::ControlInfo::ControlFlags)>();
            }
            else if(Info.Type == 2 && Info.Size == 48)
            {
                Info.ELFDigest40 = File.Read<decltype(SELF::ControlInfo::ELFDigest40)>();
            }
            else if(Info.Type == 2 && Info.Size == 64)
            {
                Info.ELFDigest30 = File.Read<decltype(SELF::ControlInfo::ELFDigest30)>();
            }
            else if(Info.Type == 3)
            {
                Info.NPDRMInfo = File.Read<decltype(SELF::ControlInfo::NPDRMInfo)>();
            }
            else
            {
                LOGF_ERROR(UNSELF, "Invalid control type of %u", Info.Type);
            }

            return Info;
        }

    public:
        bool ReadHeaders()
        {
            // seek to the front of the file and read in the SCE header
            File.Seek(0);
            auto SCEHeader = File.Read<SCE::Header>();

            LOG_DEBUG(UNSELF, "---------------------");
            LOGF_DEBUG(SCE::Header, "Magic = %u", SCEHeader.Magic);
            LOGF_DEBUG(SCE::Header, "Version = %u", SCEHeader.Version.Get());
            LOGF_DEBUG(SCE::Header, "Type = %u", SCEHeader.Type.Get());
            LOGF_DEBUG(SCE::Header, "Category = %u", SCEHeader.Category.Get());
            LOGF_DEBUG(SCE::Header, "MetadataStart = %u", SCEHeader.MetadataStart.Get());
            LOGF_DEBUG(SCE::Header, "MetadataEnd = %llu", SCEHeader.MetadataEnd.Get());
            LOGF_DEBUG(SCE::Header, "Size = %llu", SCEHeader.Size.Get());

            if(SCEHeader.Magic != "SCE\0"_U32)
            {
                LOG_ERROR(UNSELF, "Invalid SCE magic");
                return false;
            }

            // save the data we need from the header
            SCEFlags = SCEHeader.Type;
            MetadataStart = SCEHeader.MetadataStart;
            MetadataEnd = SCEHeader.MetadataEnd;

            // the SELF header is after the SCE header, so we read this in as well
            auto SELFHeader = File.Read<SELF::Header>();
            
            LOG_DEBUG(UNSELF, "---------------------");
            LOGF_DEBUG(SELF::Header, "Type = %llu", SELFHeader.Type.Get());
            LOGF_DEBUG(SELF::Header, "AInfoOffsett = %llu", SELFHeader.AInfoOffset.Get());
            LOGF_DEBUG(SELF::Header, "ELFOffset = %llu", SELFHeader.ELFOffset.Get());
            LOGF_DEBUG(SELF::Header, "PHeaderOffset = %llu", SELFHeader.PHeaderOffset.Get());
            LOGF_DEBUG(SELF::Header, "SHeaderOffset = %llu", SELFHeader.SHeaderOffset.Get());
            LOGF_DEBUG(SELF::Header, "SInfoOffset = %llu", SELFHeader.SInfoOffset.Get());
            LOGF_DEBUG(SELF::Header, "VersionOffset = %llu", SELFHeader.VersionOffset.Get());
            LOGF_DEBUG(SELF::Header, "ControlOffset = %llu", SELFHeader.ControlOffset.Get());
            LOGF_DEBUG(SELF::Header, "ControlLength = %llu", SELFHeader.ControlLength.Get());
            LOGF_DEBUG(SELF::Header, "Padding = %llu", *(U64*)SELFHeader.Padding);

            // read in the app info
            File.Seek(SELFHeader.AInfoOffset);
            auto AInfo = File.Read<AppInfo>();

            LOG_DEBUG(UNSELF, "---------------------");
            LOGF_DEBUG(AppInfo, "AuthID = %llu", AInfo.AuthID.Get());
            LOGF_DEBUG(AppInfo, "VendorID = %u", AInfo.VendorID.Get());
            LOGF_DEBUG(AppInfo, "Type = %u", AInfo.Type.Get());
            LOGF_DEBUG(AppInfo, "Version = %llu", AInfo.Version.Get());
            LOGF_DEBUG(AppInfo, "Padding = %llu", *(U64*)AInfo.Padding);

            // save the data we need
            SELFType = AInfo.Type;
            SELFVersion = AInfo.Version;

            // find the ELF header
            File.Seek(SELFHeader.ELFOffset);
            SmallELF = File.Read<ELF::SmallHeader>();

            LOG_DEBUG(UNSELF, "---------------------");
            LOGF_DEBUG(ELF::SmallHeader, "Magic = %u", SmallELF.Magic);
            LOGF_DEBUG(ELF::SmallHeader, "Class = %u", SmallELF.Class);
            LOGF_DEBUG(ELF::SmallHeader, "Endian = %u", SmallELF.Endian);
            LOGF_DEBUG(ELF::SmallHeader, "Version = %u", SmallELF.Version);
            LOGF_DEBUG(ELF::SmallHeader, "ABI = %u", SmallELF.ABI);
            LOGF_DEBUG(ELF::SmallHeader, "ABIVersion = %u", SmallELF.ABIVersion);
            LOGF_DEBUG(ELF::SmallHeader, "Padding = [%u|%u|%u|%u|%u|%u|%u]", 
                SmallELF.Padding[0], 
                SmallELF.Padding[1], 
                SmallELF.Padding[2], 
                SmallELF.Padding[3], 
                SmallELF.Padding[4], 
                SmallELF.Padding[5], 
                SmallELF.Padding[6]
            );

            if(SmallELF.Magic != "\177ELF"_U32)
            {
                LOG_ERROR(UNSELF, "Invalid ELF magic");
                return false;
            }

            // check if this is a 32 or 64 bit executable
            Arch64 = SmallELF.Class == 2;

            LOG_DEBUG(UNSELF, "---------------------");

            // 64 bit elf headers are a different size from 32 bit elf headers
            // so we need to read in the correct size
            if(Arch64)
            {
                LOG_DEBUG(ELF, "64 bit ELF file");
                BigELF64 = File.Read<ELF::BigHeader<U64>>();
                
                LOGF_DEBUG(ELF::BigHeader<U64>, "Type = %u", BigELF64.Type.Get());
                LOGF_DEBUG(ELF::BigHeader<U64>, "Machine = %u", BigELF64.Machine.Get());
                LOGF_DEBUG(ELF::BigHeader<U64>, "Version = %u", BigELF64.Version.Get());
                LOGF_DEBUG(ELF::BigHeader<U64>, "Entry = %llu", BigELF64.Entry.Get());
                LOGF_DEBUG(ELF::BigHeader<U64>, "PHOffset = %llu", BigELF64.PHOffset.Get());
                LOGF_DEBUG(ELF::BigHeader<U64>, "SHOffset = %llu", BigELF64.SHOffset.Get());
                LOGF_DEBUG(ELF::BigHeader<U64>, "Flags = %u", BigELF64.Flags.Get());
                LOGF_DEBUG(ELF::BigHeader<U64>, "HeaderSize = %u", BigELF64.HeaderSize.Get());
                LOGF_DEBUG(ELF::BigHeader<U64>, "PHEntrySize = %u", BigELF64.PHEntrySize.Get());
                LOGF_DEBUG(ELF::BigHeader<U64>, "PHCount = %u", BigELF64.PHCount.Get());
                LOGF_DEBUG(ELF::BigHeader<U64>, "SHEntrySize = %u", BigELF64.SHEntrySize.Get());
                LOGF_DEBUG(ELF::BigHeader<U64>, "SHCount = %u", BigELF64.SHCount.Get());
                LOGF_DEBUG(ELF::BigHeader<U64>, "StringIndex = %u", BigELF64.StringIndex.Get());
            }
            else
            {
                LOG_DEBUG(ELF, "32 bit ELF file");
                BigELF32 = File.Read<ELF::BigHeader<U32>>();

                LOGF_DEBUG(ELF::BigHeader<U32>, "Type = %u", BigELF32.Type.Get());
                LOGF_DEBUG(ELF::BigHeader<U32>, "Machine = %u", BigELF32.Machine.Get());
                LOGF_DEBUG(ELF::BigHeader<U32>, "Version = %u", BigELF32.Version.Get());
                LOGF_DEBUG(ELF::BigHeader<U32>, "Entry = %u", BigELF32.Entry.Get());
                LOGF_DEBUG(ELF::BigHeader<U32>, "PHOffset = %u", BigELF32.PHOffset.Get());
                LOGF_DEBUG(ELF::BigHeader<U32>, "SHOffset = %u", BigELF32.SHOffset.Get());
                LOGF_DEBUG(ELF::BigHeader<U32>, "Flags = %u", BigELF32.Flags.Get());
                LOGF_DEBUG(ELF::BigHeader<U32>, "HeaderSize = %u", BigELF32.HeaderSize.Get());
                LOGF_DEBUG(ELF::BigHeader<U32>, "PHEntrySize = %u", BigELF32.PHEntrySize.Get());
                LOGF_DEBUG(ELF::BigHeader<U32>, "PHCount = %u", BigELF32.PHCount.Get());
                LOGF_DEBUG(ELF::BigHeader<U32>, "SHEntrySize = %u", BigELF32.SHEntrySize.Get());
                LOGF_DEBUG(ELF::BigHeader<U32>, "SHCount = %u", BigELF32.SHCount.Get());
                LOGF_DEBUG(ELF::BigHeader<U32>, "StringIndex = %u", BigELF32.StringIndex.Get());
            }

            if(Arch64)
            {
                File.Seek(SELFHeader.SHeaderOffset);
                Section64Headers = new ELF::SectionHeader<U64>[BigELF64.SHCount];
                for(U32 I = 0; I < BigELF64.SHCount; I++)
                {
                    auto Sect = File.Read<ELF::SectionHeader<U64>>();
                    LOGF_DEBUG(UNSELF, "%u ---------------------", I);
                    LOGF_DEBUG(ELF::SectionHeader<U64>, "StringOffset = %u", Sect.StringOffset.Get());
                    LOGF_DEBUG(ELF::SectionHeader<U64>, "Type = %u", Sect.Type.Get());
                    LOGF_DEBUG(ELF::SectionHeader<U64>, "Flags = %llu", Sect.Flags.Get());
                    LOGF_DEBUG(ELF::SectionHeader<U64>, "VirtualAddress = %llu", Sect.VirtualAddress.Get());
                    LOGF_DEBUG(ELF::SectionHeader<U64>, "Offset = %llu", Sect.Offset.Get());
                    LOGF_DEBUG(ELF::SectionHeader<U64>, "Size = %llu", Sect.Size.Get());
                    LOGF_DEBUG(ELF::SectionHeader<U64>, "Link = %u", Sect.Link.Get());
                    LOGF_DEBUG(ELF::SectionHeader<U64>, "Info = %u", Sect.Info.Get());
                    LOGF_DEBUG(ELF::SectionHeader<U64>, "Align = %llu", Sect.Align.Get());
                    LOGF_DEBUG(ELF::SectionHeader<U64>, "EntrySize = %llu", Sect.EntrySize.Get());
                    Section64Headers[I] = Sect;
                }

                File.Seek(SELFHeader.PHeaderOffset);
                Program64Headers = new ELF::ProgramHeader<U64>[BigELF64.PHCount];
                for(U32 I = 0; I < BigELF64.PHCount; I++)
                {
                    auto Sect = File.Read<ELF::ProgramHeader<U64>>();
                    LOGF_DEBUG(UNSELF, "%u ---------------------", I);
                    LOGF_DEBUG(ELF::ProgramHeader<U64>, "Type = %u", Sect.Type.Get());
                    LOGF_DEBUG(ELF::ProgramHeader<U64>, "Flags = %u", Sect.Flags.Get());
                    LOGF_DEBUG(ELF::ProgramHeader<U64>, "Offset = %llu", Sect.Offset.Get());
                    LOGF_DEBUG(ELF::ProgramHeader<U64>, "VirtualAddress = %llu", Sect.VirtualAddress.Get());
                    LOGF_DEBUG(ELF::ProgramHeader<U64>, "PhysicalAddress = %llu", Sect.PhysicalAddress.Get());
                    LOGF_DEBUG(ELF::ProgramHeader<U64>, "FileSize = %llu", Sect.FileSize.Get());
                    LOGF_DEBUG(ELF::ProgramHeader<U64>, "MemorySize = %llu", Sect.MemorySize.Get());
                    LOGF_DEBUG(ELF::ProgramHeader<U64>, "Align = %llu", Sect.Align.Get());
                    Program64Headers[I] = Sect;
                }
            }
            else
            {
                File.Seek(SELFHeader.SHeaderOffset);
                Section32Headers = new ELF::SectionHeader<U32>[BigELF32.SHCount];
                for(U32 I = 0; I < BigELF32.SHCount; I++)
                {
                    auto Sect = File.Read<ELF::SectionHeader<U32>>();

                    Section32Headers[I] = Sect;
                }

                File.Seek(SELFHeader.PHeaderOffset);
                Program32Headers = new ELF::ProgramHeader<U32>[BigELF32.PHCount];
                for(U32 I = 0; I < BigELF32.PHCount; I++)
                {
                    auto Sect = File.Read<ELF::ProgramHeader<U32>>();

                    Program32Headers[I] = Sect;
                }
            }

            File.Seek(SELFHeader.ControlOffset);
            
            // read in each control info
            U32 I = 0;
            while(I < SELFHeader.ControlLength)
            {
                auto Info = ReadControlInfo();
                I += Info.Size;
                Controls.Append(Info);
            }

            return true;
        }

    private:
        bool DecryptNPDRM(Byte* Metadata, U32 Len, Byte* MetaKey)
        {
            Byte Key[16];
            Byte IV[16];
            SELF::ControlInfo* Control = nullptr;

            for(U32 I = 0; I < Controls.Len(); I++)
            {
                if(Controls[I].Type == 3)
                {
                    Control = &Controls[I];
                    break;
                }
            }

            if(!Control)
            {
                return true;
            }

            if(Control->NPDRMInfo.Version == 1)
            {
                LOG_ERROR(UNSELF, "Cannot decrypt network license");
                return false;
            }
            else if(Control->NPDRMInfo.Version == 2)
            {
                //TODO: support local licenses
                LOG_ERROR(UNSELF, "Local licenses are not supported yet");
                return false;
            }
            else if(Control->NPDRMInfo.Version == 3)
            {
                if(MetaKey)
                    Memory::Copy<Byte>(Keys::FreeKlic, Key, 16);
                else
                    Memory::Copy<Byte>(MetaKey, Key, 16);
            }

            aes_context AES;

            aes_setkey_dec(&AES, Key, 128);
            aes_crypt_cbc(&AES, AES_DECRYPT, Len, IV, Metadata, Metadata);

            return true;
        }

    public:
        bool ReadMetadata(Byte* Key)
        {
            File.Seek(MetadataStart + sizeof(SCE::Header));
            auto MetaInfo = File.Read<MetaData::Info>();

            for(Byte B : MetaInfo.IV)
            {
                printf("%u ", B);
            }
            printf("\n");

            for(Byte B : MetaInfo.IVPad)
            {
                printf("%u ", B);
            }
            printf("\n");

            for(Byte B : MetaInfo.Key)
            {
                printf("%u ", B);
            }
            printf("\n");

            for(Byte B : MetaInfo.KeyPad)
            {
                printf("%u ", B);
            }
            printf("\n\n");

            aes_context AES;
            SELF::Key MetaKey = GetSELFKey(static_cast<KeyType>(SELFType), SCEFlags, SELFVersion);

            if((SCEFlags & 0x8000) != 0x8000)
            {
                if(!DecryptNPDRM((Byte*)&MetaInfo, sizeof(MetaData::Info), Key))
                    return false;

                aes_setkey_dec(&AES, MetaKey.ERK, 256);
                aes_crypt_cbc(&AES, AES_DECRYPT, sizeof(MetaData::Info), MetaKey.RIV, (Byte*)&MetaInfo, (Byte*)&MetaInfo);
            }

            for(Byte B : MetaInfo.IV)
            {
                printf("%u ", B);
            }
            printf("\n");

            for(Byte B : MetaInfo.IVPad)
            {
                printf("%u ", B);
            }
            printf("\n");

            for(Byte B : MetaInfo.Key)
            {
                printf("%u ", B);
            }
            printf("\n");

            for(Byte B : MetaInfo.KeyPad)
            {
                printf("%u ", B);
            }
            printf("\n");

            for(U32 I = 0; I < sizeof(MetaInfo.Key); I++)
            {
                if(MetaInfo.KeyPad[I] | MetaInfo.IVPad[0])
                {
                    LOG_ERROR(UNSELF, "Failed to decrypt metadata info");
                    return false;
                }
            }

            const U32 HeaderSize = MetadataEnd - sizeof(SCE::Header) - MetadataStart - sizeof(MetaData::Info);
            Headers = new Byte[HeaderSize];
            File.ReadN(Headers, HeaderSize);

            size_t Offset = 0;
            Byte Stream[16];
            aes_setkey_enc(&AES, MetaInfo.Key, 128);
            aes_crypt_ctr(&AES, HeaderSize, &Offset, MetaInfo.IV, Stream, Headers, Headers);

            const auto MetaHead = *(MetaData::Header*)Headers;
            Headers += sizeof(MetaData::Header);

            LOGF_DEBUG(MetaData::Header, "SignatureLength = %llu", MetaHead.SignatureLength.Get());
            LOGF_DEBUG(MetaData::Header, "AlgorithmType = %u", MetaHead.AlgorithmType.Get());
            LOGF_DEBUG(MetaData::Header, "SectionCount = %u", MetaHead.SectionCount.Get());
            LOGF_DEBUG(MetaData::Header, "KeyCount = %u", MetaHead.KeyCount.Get());
            LOGF_DEBUG(MetaData::Header, "HeaderSize = %u", MetaHead.HeaderSize.Get());
            LOGF_DEBUG(MetaData::Header, "Padding = [%u|%u|%u|%u|%u|%u|%u|%u]", 
                MetaHead.Padding[0], 
                MetaHead.Padding[1], 
                MetaHead.Padding[2], 
                MetaHead.Padding[3], 
                MetaHead.Padding[4], 
                MetaHead.Padding[5], 
                MetaHead.Padding[6], 
                MetaHead.Padding[7]
            );

            DataBufferLength = MetaHead.KeyCount * 16;

            for(U32 I = 0; I < MetaHead.SectionCount; I++)
            {
                auto Section = *(MetaData::Section*)(Headers + sizeof(MetaData::Section) * I);
                
                LOGF_DEBUG(UNSELF, "%u -------------------------", I);
                LOGF_DEBUG(MetaData::Section, "Offset = %llu", Section.Offset.Get());
                LOGF_DEBUG(MetaData::Section, "Size = %llu", Section.Size.Get());
                LOGF_DEBUG(MetaData::Section, "Type = %u", Section.Type.Get());
                LOGF_DEBUG(MetaData::Section, "Index = %u", Section.Index.Get());
                LOGF_DEBUG(MetaData::Section, "HashAlgo = %u", Section.HashAlgo.Get());
                LOGF_DEBUG(MetaData::Section, "HashIndex = %u", Section.HashIndex.Get());
                LOGF_DEBUG(MetaData::Section, "Encrypted = %u", Section.Encrypted.Get());
                LOGF_DEBUG(MetaData::Section, "KeyIndex = %u", Section.KeyIndex.Get());
                LOGF_DEBUG(MetaData::Section, "IVIndex = %u", Section.IVIndex.Get());
                LOGF_DEBUG(MetaData::Section, "Compressed = %u", Section.Compressed.Get());
                
                if(Section.Encrypted == 3)
                    DataBufferLength += Section.Size;

                MetaSections.Append(Section);
            }

            Headers += MetaHead.SectionCount * sizeof(MetaData::Section);

            return true;
        }

        void DecryptData()
        {
            aes_context AES;

            U32 BufferOffset = 0;

            Byte* DataBuffer = new Byte[DataBufferLength];
            Byte* Keys = Headers;

            for(auto Section : MetaSections)
            {
                Byte Key[16];
                Byte IV[16];
                Byte Stream[16];
                size_t Offset = 0;

                if(Section.Encrypted == 3)
                {  
                    Memory::Copy(Keys + Section.KeyIndex * 16, Key, 16);
                    Memory::Copy(Keys + Section.IVIndex * 16, IV, 16);

                    Byte* Buffer = new Byte[Section.Size];

                    File.Seek(Section.Offset);
                    File.ReadN(Buffer, Section.Size);

                    Memory::Zero(Stream, 16);

                    aes_setkey_enc(&AES, Key, 128);
                    aes_crypt_ctr(&AES, Section.Size, &Offset, IV, Stream, Buffer, Buffer);
            
                    Memory::Copy(Buffer, DataBuffer + BufferOffset, Section.Size);

                    BufferOffset += Section.Size;
                }
            }
        }

        template<typename TSections, typename THeader, typename TPrograms>
        ELF::Binary CreateELF(THeader* Header, TSections* Sections, TPrograms* Programs)
        {
            ELF::Binary Bin = {128};

            Bin.Write(&SmallELF);
            Bin.Write(Header);

            return Bin;
        }
    };

    namespace UNSELF
    {
        Cthulhu::Option<ELF::Binary> DecryptSELF(Cthulhu::FileSystem::BufferedFile& File, Byte* Key)
        {
            SELFDecryptor Decrypt(File);

            if(!Decrypt.ReadHeaders())
            {
                return None<ELF::Binary>();
            }

            if(!Decrypt.ReadMetadata(Key))
            {
                return None<ELF::Binary>();
            }

            Decrypt.DecryptData();

            return Some(Decrypt.CreateELF());
        }
    }
}

#if 0
namespace Volts::PS3
{
    using namespace Cthulhu;
    // SELF files are big, complicated files that were designed to not be easy to decrypt
    // this code is going to reflect the many stages required to bypass all of sonys security
    namespace SCE
    {
        struct Header
        {
            // should always be 4539219ULL or "SCE\0"
            U32 Magic;
            
            // will be 2 for a PS3 game and 3 for a PSVita game
            Big<U32> HeaderVersion;

            Big<U16> KeyType;
            Big<U16> FileCategory;
            Big<U32> MetadataStart;
            Big<U64> MetadataEnd;
            Big<U64> DataLength;
        };

        static_assert(sizeof(SCE::Header) == 32);

        struct VersionInfo
        {
            Big<U32> SubType;
            Big<U32> Present;
            Big<U32> Size;
            U8 Padding[4];
        };

        static_assert(sizeof(VersionInfo) == 16);
    }

    namespace ELF
    {
        struct SmallHeader
        {
            Cthulhu::U32 Magic;
            Cthulhu::U8 Class;
            Cthulhu::U8 Endianness;
            Cthulhu::U8 Version;
            Cthulhu::U8 ABI;
            Cthulhu::U8 ABIVersion;
            Cthulhu::U8 Padding[7];
        };

        static_assert(sizeof(SmallHeader) == 16);

        template<typename T>
        struct BigHeader
        {
            Big<Cthulhu::U16> Type;
            Big<Cthulhu::U16> Machine;
            Big<Cthulhu::U32> Version;
            
            Big<T> Entry;
            Big<T> PHOffset;
            Big<T> SHOffset;
            
            Big<Cthulhu::U32> Flags;
            Big<Cthulhu::U16> HeaderSize;
            
            Big<Cthulhu::U16> PHEntrySize;
            Big<Cthulhu::U16> PHCount;
            
            Big<Cthulhu::U16> SHEntrySize;
            Big<Cthulhu::U16> SHCount;
            Big<Cthulhu::U16> StringTableIndex;
        };

        static_assert(sizeof(BigHeader<U32>) == 52 - sizeof(SmallHeader));
        static_assert(sizeof(BigHeader<U64>) == 64 - sizeof(SmallHeader));

        template<typename T>
        struct ProgramHeader {};

        template<> 
        struct ProgramHeader<Cthulhu::U32> 
        {
            Big<Cthulhu::U32> Type;

            Big<Cthulhu::U32> Offset;
            Big<Cthulhu::U32> VirtualAddress;
            Big<Cthulhu::U32> PhysicalAddress;
            Big<Cthulhu::U32> FileSize;
            Big<Cthulhu::U32> MemorySize;

            Big<Cthulhu::U32> Flags;
            Big<Cthulhu::U32> Align;
        };

        static_assert(sizeof(ProgramHeader<U32>) == 32);

        template<> 
        PACKED_STRUCT(ProgramHeader<Cthulhu::U64>,
        {
            Big<Cthulhu::U32> Type;
            Big<Cthulhu::U32> Flags;

            Big<Cthulhu::U64> Offset;
            Big<Cthulhu::U64> VirtualAddress;
            Big<Cthulhu::U64> PhysicalAddress;
            Big<Cthulhu::U64> FileSize;
            Big<Cthulhu::U64> MemorySize;
            
            Big<Cthulhu::U64> Align;
        })

        static_assert(sizeof(ProgramHeader<U64>) == 56);

        template<typename T>
        struct SectionHeader
        {
            Big<Cthulhu::U32> NameOffset;
            Big<Cthulhu::U32> Type;

            Big<T> Flags;
            Big<T> VirtualAddress;
            Big<T> Offset;
            Big<T> Size;

            Big<Cthulhu::U32> Link;
            Big<Cthulhu::U32> Info;

            Big<T> Align;
            Big<T> EntrySize;
        };

        static_assert(sizeof(SectionHeader<U32>) == 40);
        static_assert(sizeof(SectionHeader<U64>) == 64);

    }


    namespace SELF
    {
        struct Header
        {
            Big<U64> Type;
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

        static_assert(sizeof(SELF::Header) == 80);

        struct Section
        {
            Big<U64> Offset;
            Big<U64> Size;
            Big<U32> Compressed;
            Byte Padding[8];
            Big<U32> Encrypted;
        };
    }

    struct AppInfo
    {
        Big<U64> AuthID;
        Big<U32> VendorID;
        Big<KeyType> Type;
        Big<U32> Version;
        U8 Padding[12];
    };

    static_assert(sizeof(AppInfo) == 32);

    struct SectionInfo
    {
        Big<U64> Offset;
        Big<U64> Size;
        Big<U32> Compressed; // 1 if uncompressed, 2 if compressed
        U8 Padding[8]; // should always be 0
        Big<U32> Encrypted; // 1 if encrypted, 2 if unecrypted
    };

    static_assert(sizeof(SectionInfo) == 32);

    struct ControlInfo
    {
        U32 Type;
        U32 Size;
        U64 HasNext;

        union
        {
            struct
            {
                Big<U32> ControlFlag;
                U8 Padding[28];
            } ControlFlags;

            static_assert(sizeof(ControlFlags) == 32);

            struct
            {
                U8 Constant[20];
                U8 Digest[20];
                Big<U64> RequiredSystemVersion;
            } ELFDigest40;

            static_assert(sizeof(ELFDigest40) == 48);

            struct
            {
                U8 ConstantOrDigest[20];
                U8 Padding[12];
            } ELFDigest30;

            static_assert(sizeof(ELFDigest30) == 32);

            struct
            {
                U32 Magic;
                Big<U32> LicenseVersion;
                Big<U32> DRMType;
                Big<U32> AppType;
                U8 ContentID[48];
                U8 Digest[16];
                U8 INVDigest[16];
                U8 XORDigest[16];
                U8 Padding[16];
            } NPDRMInfo;

            static_assert(sizeof(NPDRMInfo) == 128);
        };
    };

    namespace MetaData
    {
        struct Header
        {
            Big<U64> SignatureLength;
            U8 Padding1[4];
            Big<U32> SectionCount;
            Big<U32> KeyCount;
            Big<U32> HeaderSize;
            U8 Padding2[8];
        };

        static_assert(sizeof(Header) == 32);

        struct Section
        {
            Big<U64> Offset;
            Big<U64> Size;
            Big<U32> Type;
            Big<U32> ProgramIndex;
            Big<U32> HashAlgo;
            Big<U32> HashIndex;
            Big<U32> Encrypted;
            Big<U32> KeyIndex;
            Big<U32> IVIndex;
            Big<U32> Compressed;
        };

        static_assert(sizeof(Section) == 48);

        struct Info
        {
            U8 Key[16];
            U8 KeyPadding[16];

            U8 IV[16];
            U8 IVPadding[16];
        };

        static_assert(sizeof(Info) == 64);
    }

    struct Decryptor
    {
        Decryptor(FS::BufferedFile& F)
            : File(F)
        {}

        ~Decryptor()
        {
            if(Is32)
            {
                delete PHeaders32;
                delete SHeaders32;
            }
            else
            {
                delete PHeaders64;
                delete SHeaders64;
            }

            delete[] KeyBuffer;
            delete[] DataBuffer;
        }

    public:
        ControlInfo ReadControlInfo()
        {           
            // read into a ControlInfo header 
            ControlInfo CTRL;
            
            // as this structure has to be read in 2 parts we read in
            // the initial data seperatley so we dont read too many bytes
            CTRL.Type = File.Read<Big<U32>>();
            CTRL.Size = File.Read<Big<U32>>();
            CTRL.HasNext = File.Read<Big<U64>>();

            // next we have to check what kind of info this header has
            // then read in the relevant sort of data
            if(CTRL.Type == 1)
            {   
                CTRL.ControlFlags = File.Read<decltype(CTRL.ControlFlags)>();
            }
            else if(CTRL.Type == 2)
            {
                if(CTRL.Size == 48)
                {
                    CTRL.ELFDigest30 = File.Read<decltype(CTRL.ELFDigest30)>();
                }
                else if(CTRL.Size == 64)
                {
                    CTRL.ELFDigest40 = File.Read<decltype(CTRL.ELFDigest40)>();
                }
            }
            else if(CTRL.Type == 3)
            {
                CTRL.NPDRMInfo = File.Read<decltype(CTRL.NPDRMInfo)>();
            }
            else
            {
                LOGF_ERROR(UNSELF, "Invalid Control Type of %u", CTRL.Type);
            }

            return CTRL;
        }

        bool LoadHeaders() 
        {
            // seek to the front of the file so we start at the sce header
            File.Seek(0);

            // Read in the SCE header
            // this is essentially metadata the ps3 uses internally
            // we need some, but not all of it
            SCEHead = File.Read<SCE::Header>();

            // check the SCE magic
            if(SCEHead.Magic != "SCE\0"_U32)
            {
                LOG_ERROR(UNSELF, "Invalid SCE Header magic");
                return false;
            }

            // read in the self header
            SELFHead = File.Read<SELF::Header>();
            
            // seek to the app info and read it in
            File.Seek(SELFHead.InfoOffset);
            auto Info = File.Read<AppInfo>();

            // we only need these two variables so we only store these 2
            InfoType = Info.Type;
            InfoVersion = Info.Version;

            // read in a small elf header so we know how the rest of the header is formatted
            File.Seek(SELFHead.ELFOffset);
            Small = File.Read<ELF::SmallHeader>();

            // check the magic of the ELF small header
            if(Small.Magic != "\177ELF"_U32)
            {
                LOG_ERROR(UNSELF, "ELF Header had incorrect magic");
                return false;
            }

            // as this is an ELF header it is actually comprised of 2
            // seperate headers glued together, this allows up to parse more effectivley
            // if the ELFs "class" is 1 then it is a 32 bit ELF binary
            if(Small.Class == 1)
            {
                Is32 = true;
                // is 32 bit
                ELFHead32 = File.Read<ELF::BigHeader<U32>>();
            }
            else
            {
                Is32 = false;
                // must be 64 bit otherwise
                ELFHead64 = File.Read<ELF::BigHeader<U64>>();


                //LOGF_DEBUG(UNSELF, "Type = %u", ELFHead64.Type.Get());
                //LOGF_DEBUG(UNSELF, "Machine = %u", ELFHead64.Machine.Get());
                //LOGF_DEBUG(UNSELF, "Version = %u", ELFHead64.Version.Get());
                //LOGF_DEBUG(UNSELF, "Entry = %llu", ELFHead64.Entry.Get());
                //LOGF_DEBUG(UNSELF, "POff = %llu", ELFHead64.PHOffset.Get());
                //LOGF_DEBUG(UNSELF, "SOff = %llu", ELFHead64.SHOffset.Get());
            }

            // seek to the start of the program headers
            //File.Seek(SELFHead.ProgramHeaderOffset);
            //LOGF_DEBUG(UNSELF, "PHOffset = %llu", SELFHead.ProgramHeaderOffset.Get());

            LOGF_DEBUG(UNSELF, "Depth = %u", File.CurrentDepth());

            // the amount of program headers we need to read in, this is used later but is set here
            // to avoid pointless branching
            U32 PHCount;

            // if the elf file is 32 bit
            if(Is32)
            {
                // set the amount of program headers if we're using 32 bit arch
                PHCount = ELFHead32.PHCount;
                
                // load program headers and then section headers
                PHeaders32 = new Array<ELF::ProgramHeader<U32>>();

                // read in each program header, they're packed next to each other so no seeking is needed
                for(U32 I = 0; I < PHCount; I++)
                {
                    PHeaders32->Append(File.Read<ELF::ProgramHeader<U32>>());
                }

                // create the section header array
                SHeaders32 = new Array<ELF::SectionHeader<U32>>();

                // the section headers are not next to the program headers, so we need to seek to them
                File.Seek(SELFHead.SectionHeaderOffset);

                // now read in each section header, these are also packed so they can be read in without seeking
                for(U32 I = 0; I < ELFHead32.SHCount; I++)
                {
                    SHeaders32->Append(File.Read<ELF::SectionHeader<U32>>());
                }
            }
            else 
            {
                PHCount = ELFHead64.PHCount;

                PHeaders64 = new Array<ELF::ProgramHeader<U64>>();

                LOGF_DEBUG(UNSELF, "PCount = %u", PHCount);

                for(U32 I = 0; I < PHCount; I++)
                {
                    auto PHead = File.Read<ELF::ProgramHeader<U64>>();

                    LOGF_DEBUG(UNSELF, "%u = %llu", I, PHead.Offset.Get());

                    PHeaders64->Append(PHead);
                }

                SHeaders64 = new Array<ELF::SectionHeader<U64>>();

                File.Seek(SELFHead.SectionHeaderOffset);

                for(U32 I = 0; I < ELFHead64.SHCount; I++)
                {
                    SHeaders64->Append(File.Read<ELF::SectionHeader<U64>>());
                }
            }
            
            // read in the version header
            File.Seek(SELFHead.VersionOffset);
            SCEVer = File.Read<decltype(SCEVer)>();
            
            // read control info
            File.Seek(SELFHead.ControlOffset);

            // only read in while I is less than the total length of all control info headers
            U32 I = 0;
            while(I < SELFHead.ControlLength)
            {
                // read in a single control header
                auto CInfo = ReadControlInfo();
                CInfoArray.Append(CInfo);
                // add the length of the object to I so we know how much size we have left
                I += CInfo.Size;
            }

            // read in the section info headers
            File.Seek(SELFHead.SectionInfoOffset);

            // read in each section info, these are packed together so no need to seek each time
            for(U32 I = 0; I < PHCount; I++)
            {
                SInfoArray.Append(File.Read<SectionInfo>());
            }

            return true;
        }

    private:
        bool KeyFromRAP(U8* ID, U8* Key)
        {
            // TODO: this needs a whole ton of other things before it will work properly
            return false;
        }

        bool DecryptNPDRM(U8* Metadata, U32 Len, U8* MetaKey = nullptr)
        {
            ControlInfo* Control = nullptr;
            U8 Key[16];
            U8 IV[16];

            for(U32 I = 0; I < CInfoArray.Len(); I++)
            {
                if(CInfoArray[I].Type == 3)
                {
                    Control = &CInfoArray[I];
                    break;
                }
            }

            if(!Control)
            {
                return true;
            }

            if(Control->NPDRMInfo.LicenseVersion == 1)
            {
                LOG_ERROR(UNSELF, "Cannot decrypt network NPDRM license");
                return false;
            }
            else if(Control->NPDRMInfo.LicenseVersion == 2)
            {
                // TODO: support these things
                LOG_ERROR(UNSELF, "Local Licenses not supported yet");
                return false;
            }
            else if(Control->NPDRMInfo.LicenseVersion == 3)
            {
                if(MetaKey != nullptr)
                    Memory::Copy<U8>(MetaKey, Key, 16);
                else
                    Memory::Copy<U8>(Keys::FreeKlic, Key, 16);
            }

            aes_context AES;

            aes_setkey_dec(&AES, Keys::Klic, 128);
            aes_crypt_ecb(&AES, AES_DECRYPT, Key, Key);

            Memory::Zero<U8>(IV, 16);

            aes_setkey_dec(&AES, Key, 128);
            aes_crypt_cbc(&AES, AES_DECRYPT, Len, IV, Metadata, Metadata);
        
            return true;
        }
    public:

        bool LoadData(U8* Key) 
        {            
            File.Seek(SCEHead.MetadataStart + sizeof(SCE::Header));
            auto MetaInfo = File.Read<MetaData::Info>();
            
            aes_context AES;
            
            SELF::Key MetaKey = GetSELFKey(InfoType, SCEHead.KeyType, InfoVersion);

            if((SCEHead.KeyType & 0x8000) != 0x8000)
            {
                // isnt a debug self, stuff is encrypted
                if(!DecryptNPDRM((U8*)&MetaInfo, sizeof(MetaData::Info), Key))
                    return false;

                aes_setkey_dec(&AES, MetaKey.ERK, 256);
                aes_crypt_cbc(&AES, AES_DECRYPT, sizeof(MetaData::Info), MetaKey.RIV, (U8*)&MetaInfo, (U8*)&MetaInfo);
            }

            if(MetaInfo.IVPadding[0] | MetaInfo.KeyPadding[0])
            {
                LOG_ERROR(UNSELF, "Failed to decrypt MetaData Info");
                return false;
            }

            const U32 HSize = SCEHead.MetadataEnd - (sizeof(SCE::Header) + SCEHead.MetadataStart + sizeof(MetaData::Info));
            Byte* Headers = new Byte[HSize];

            File.Seek(SCEHead.MetadataStart + sizeof(SCE::Header) + sizeof(MetaData::Info));
            File.ReadN(Headers, HSize);

            size_t Offset = 0;
            Byte Stream[16];
            aes_setkey_enc(&AES, MetaInfo.Key, 128);
            aes_crypt_ctr(&AES, HSize, &Offset, MetaInfo.IV, Stream, Headers, Headers);

            const auto MetaHead = *(MetaData::Header*)Headers;
            Headers += sizeof(MetaData::Header);

            LOGF_DEBUG(UNSELF, "MetaSectionCount = %u", MetaHead.SectionCount.Get());

            MetaKeyCount = MetaHead.KeyCount;
            MetaSectionCount = MetaHead.SectionCount;

            DataLength = 0;

            for(U32 I = 0; I < MetaHead.SectionCount; I++)
            {
                const auto Section = *(MetaData::Section*)(Headers + sizeof(MetaData::Section) * I);

                // check if this section is even important, if it isnt then we dont need to append it or proccess it
                if(Section.Encrypted == 3 && (Section.KeyIndex <= MetaHead.KeyCount - 1) && (Section.IVIndex <= MetaHead.KeyCount))
                    DataLength += Section.Size;

                MetaSections.Append(Section);
            }

            DataBuffer = new U8[DataLength];

            KeyLength = MetaKeyCount * 16;
            KeyBuffer = new Byte[KeyLength];
            Memory::Copy<Byte>((Headers + sizeof(MetaData::Header) + MetaHead.SectionCount * sizeof(MetaData::Section)), KeyBuffer, KeyLength);

            return true;
        }

        void DecryptData() 
        { 
            aes_context AES;

            size_t Offset = 0;

            for(const auto& Section : MetaSections)
            {
                size_t NCOffset = 0;
            
                Byte Key[16];
                Byte IV[16];

                if(Section.Encrypted == 3 && (Section.KeyIndex <= MetaKeyCount - 1) && (Section.IVIndex <= MetaKeyCount))
                {
                    Byte Stream[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
                    Memory::Copy<Byte>(KeyBuffer + Section.KeyIndex * 16, Key, 16);
                    Memory::Copy<Byte>(KeyBuffer + Section.IVIndex * 16, IV, 16);

                    File.Seek(Section.Offset);
                    
                    Byte* Data = new Byte[Section.Size];
                    File.ReadN(Data, Section.Size);

                    LOGF_DEBUG(UNSELF, "Encrypt %u %u %u", Data[0], Data[1], Data[2]);

                    aes_setkey_enc(&AES, Key, 128);
                    aes_crypt_ctr(&AES, Section.Size, &NCOffset, IV, Stream, Data, Data);

                    Memory::Copy<Byte>(Data, DataBuffer + Offset, Section.Size);

                    LOGF_DEBUG(UNSELF, "Decrypt %u %u %u", (DataBuffer + Offset)[0], (DataBuffer + Offset)[1], (DataBuffer + Offset)[2]);

                    Offset += Section.Size;
                }
            }
        }

    private:
        template<typename THeader, typename TProgram, typename TSection, typename TOffset>
        ELF::Binary CreateBinary(THeader Head, TProgram& Programs, TSection& Sections, TOffset SHOffset)
        {
            ELF::Binary Bin = {128};

            // write out the ELF header
            Bin.Write(&Small);
            Bin.Write(&Head);

            // write out the program sections 
            for(auto Program : Programs)
            {
                Bin.Write(&Program);
            }

            U32 Offset = 0;

            for(auto Sect : MetaSections)
            {
                if(Sect.Type == 2)
                {
                    Bin.Seek(Programs[Sect.ProgramIndex].Offset);
                    LOGF_DEBUG(UNSELF, "ProgramIndex = %u", Sect.ProgramIndex.Get());
                    LOGF_DEBUG(UNSELF, "Offset = %llu", Programs[Sect.ProgramIndex].Offset.Get());
                    LOGF_DEBUG(UNSELF, "Size = %llu", Sect.Size.Get());

                    if(Sect.Compressed == 2)
                    {
                        const U32 Size = Programs[Sect.ProgramIndex].FileSize;
                        Byte* ZBuffer = new Byte[Size];

                        uLongf ZLength = static_cast<uLongf>(Size);

                        uncompress(ZBuffer, &ZLength, DataBuffer + Offset, DataLength);

                        LOGF_DEBUG(UNSELF, "COffset = %llu", Programs[Sect.ProgramIndex].Offset.Get());
                        Bin.Seek(Programs[Sect.ProgramIndex].Offset);
                        LOGF_DEBUG(UNSELF, "Position = %u", Bin.Depth());
                        Bin.Write(ZBuffer, Size);
                        LOGF_DEBUG(UNSELF, "Position After = %u", Bin.Depth());
                    }
                    else
                    {
                        LOGF_DEBUG(UNSELF, "NCOffset = %llu", Programs[Sect.ProgramIndex].Offset.Get());
                        Bin.Seek(Programs[Sect.ProgramIndex].Offset);
                        LOGF_DEBUG(UNSELF, "Position = %u", Bin.Depth());
                        Bin.Write(DataBuffer + Offset, Sect.Size);
                        //LOGF_DEBUG(UNSELF, "Position After = %u", Bin.Depth());
                    }

                    if(Bin.GetData()[0] != '\177')
                    {
                        LOG_ERROR(UNSELF, "Did a bad");
                        LOG_ERROR(UNSELF, "---------");
                    }
                    
                    Offset += Sect.Size;
                }
            }

            if(SELFHead.SectionHeaderOffset != 0)
            {
                Bin.Seek(SHOffset);

                for(auto SHead : Sections)
                {
                    Bin.Write(&SHead);
                }
            }

            return Bin;
        }

        // write data to the file but dont overwrite existing data
        void WriteUnder(ELF::Binary& Bin, Byte* Data, U64 Size)
        {
            const U64 Overlap = Math::Min<U64>(Bin.GetLength() - Bin.Depth(), Size);

            Byte* Temp = Memory::Duplicate(Bin.TakeData(), Bin.GetLength() + Size);
            Memory::Copy(Data, Temp + Bin.Depth(), Overlap);

            Bin.Write(Temp + Overlap, Size);
#if 0
            Byte* Temp = Bin.TakeData();
            U64 BinLength = PrePos;
            U64 Pos = Bin.Depth();

            Byte* NewData = Memory::Duplicate(Temp, BinLength);
            Bin.Write(Data, Size);
            U64 Actual = Bin.Depth();
            Bin.Seek(Pos);
            Bin.Write(NewData, BinLength);
#endif
        }

    public:

        ELF::Binary ToBinary()
        {
            U32 Idx = 0;
            for(auto PHead : *PHeaders64)
            {
                LOGF_DEBUG(UNSELF, "%u = %llu", Idx++, PHead.Offset.Get());
            }

            if(Is32)
                ; //return CreateBinary(ELFHead32, *PHeaders32, *SHeaders32, ELFHead32.SHOffset.Get());
            else
                return CreateBinary(ELFHead64, *PHeaders64, *SHeaders64, ELFHead64.SHOffset.Get());
        }

        bool Is32;

        FS::BufferedFile& File;

        SCE::Header SCEHead;
        SELF::Header SELFHead;
        KeyType InfoType;
        U32 InfoVersion;

        Array<SectionInfo> SInfoArray;
        Array<ControlInfo> CInfoArray;

        SCE::VersionInfo SCEVer;

        ELF::SmallHeader Small;

        union
        {
            ELF::BigHeader<U32> ELFHead32;
            ELF::BigHeader<U64> ELFHead64;
        };

        union
        {
            struct
            {
                Array<ELF::SectionHeader<U32>>* SHeaders32;
                Array<ELF::ProgramHeader<U32>>* PHeaders32;
            };

            struct
            {
                Array<ELF::SectionHeader<U64>>* SHeaders64;
                Array<ELF::ProgramHeader<U64>>* PHeaders64;
            };
        };

        U32 MetaKeyCount;
        U32 MetaSectionCount;
        MetaData::Info MetaInfo;
        Array<MetaData::Section> MetaSections;

        U32 KeyLength;
        Byte* KeyBuffer;

        U32 DataLength;
        Byte* DataBuffer;
    };

    namespace UNSELF
    {
        // file format reference from https://www.psdevwiki.com/ps3/SELF_File_Format_and_Decryption#Extracting_an_ELF
        Option<ELF::Binary> DecryptSELF(FS::BufferedFile& File, U8* Key)
        {
            Decryptor Decrypt(File);

            if(!Decrypt.LoadHeaders())
            {
                return None<ELF::Binary>();
            }

            if(!Decrypt.LoadData(Key))
            {
                return None<ELF::Binary>();
            }

            Decrypt.DecryptData();

            return Some(Decrypt.ToBinary());
        }
    }
}

#endif