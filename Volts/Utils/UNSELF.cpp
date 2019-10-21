#include "UNSELF.h"

#include "Core/Emulator.h"
#include "Core/Endian.h"
#include "Core/Convert.h"

#include "Keys.h"

#include <vector>

#include "aes/aes.h"

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

        MetaData::Header MetaHead;

        Byte* DataKeys;
        U32 DataKeysLength;

        Byte* DataBuffer;
        U32 DataLength;

        std::vector<MetaData::Section> MetaSections;

        std::vector<ELF::SectionHeader> SHeaders;
        std::vector<ELF::ProgramHeader> PHeaders;

        std::vector<SELF::ControlInfo> Controls;

        bool DecryptNPDRM(Byte* Metadata, U32 Len, Byte* MetaKey)
        {
            SELF::ControlInfo* Ctrl = nullptr;

            for(U32 I = 0; I < Controls.size(); I++)
                if(Controls[I].Type == 3)
                {
                    Ctrl = &Controls[I];
                    break;
                }

            if(!Ctrl)
                return true;

            Byte Key[16];
            switch(Ctrl->NPDRMInfo.Version)
            {
            case 1:
                VERROR("Cannot decrypt network license");
                return false;
            case 2:
                VERROR("Local licenses are not supported yet");
                return false;
            case 3:
                Memory::Copy<Byte>(MetaKey ? Keys::FreeKlic : MetaKey, Key, 16);
                return true;
            default:
                VERROR("Invalid license type {}", Ctrl->NPDRMInfo.Version.Get());
                return false;
            }
        }

        SELF::ControlInfo ReadControlInfo()
        {
            SELF::ControlInfo Ctrl;

            Ctrl.Type = File.Read<Big<U32>>();
            Ctrl.Size = File.Read<Big<U32>>();
            Ctrl.Next = File.Read<Big<U64>>();

            if(Ctrl.Type == 1)
                Ctrl.ControlFlags = File.Read<decltype(SELF::ControlInfo::ControlFlags)>();
            else if(Ctrl.Type == 2 && Ctrl.Size == 48)
                Ctrl.ELFDigest40 = File.Read<decltype(SELF::ControlInfo::ELFDigest40)>();
            else if(Ctrl.Type == 2 && Ctrl.Size == 64)
                Ctrl.ELFDigest30 = File.Read<decltype(SELF::ControlInfo::ELFDigest30)>();
            else if(Ctrl.Type == 3)
                Ctrl.NPDRMInfo = File.Read<decltype(SELF::ControlInfo::NPDRMInfo)>();
            else
                VERROR("Invalid control type of %u", Ctrl.Type);

            return Ctrl;
        }

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

            if(ELFHead.Magic != "\177ELF"_U32)
            {
                VERROR("Invalid ELF magic");
                return false;
            }

            for(U32 I = 0; I < ELFHead.PHCount; I++)
                PHeaders.push_back(File.Read<ELF::ProgramHeader>());

            File.Seek(SELFHead.ControlOffset);

            U32 C = 0;
            while(C < SELFHead.ControlLength)
            {
                auto Ctrl = ReadControlInfo();
                C += Ctrl.Size;

                Controls.push_back(Ctrl);
            }

            File.Seek(SELFHead.SHeaderOffset);

            for(U32 I = 0; I < ELFHead.SHCount; I++)
                SHeaders.push_back(File.Read<ELF::SectionHeader>());

            return true;
        }

        bool ReadMetadata(Byte* Key)
        {
            File.Seek(SCEHead.MetadataStart + sizeof(SCE::Header));

            auto MetaInfo = File.Read<MetaData::Info>();

            const U32 HeaderSize = SCEHead.MetadataEnd 
                - sizeof(SCE::Header) 
                - SCEHead.MetadataStart 
                - sizeof(MetaData::Info);

            Byte* Headers = new Byte[HeaderSize];

            File.Seek(SCEHead.MetadataStart + sizeof(SCE::Header) + sizeof(MetaData::Info));

            File.ReadN(Headers, HeaderSize);

            aes_context AES;
            return false;

            SELF::Key MetaKey = GetSELFKey((KeyType)Info.Type.Get(), SCEHead.Type, Info.Version);

            if((SCEHead.Type & 0x8000) != 0x8000)
            {
                if(!DecryptNPDRM((Byte*)&MetaInfo, sizeof(MetaData::Info), Key))
                    return false;

                aes_setkey_dec(&AES, MetaKey.ERK, 256);
                aes_crypt_cbc(&AES, AES_DECRYPT, sizeof(MetaData::Info), MetaKey.RIV, (Byte*)&MetaInfo, (Byte*)&MetaInfo);
            }

            for(U32 I = 0; I < 16; I++)
                if(MetaInfo.KeyPad[I] | MetaInfo.IVPad[I])
                {
                    VERROR("Failed to decrypt metadata info");
                    return false;
                }

            size_t Offset = 0;
            Byte Stream[16] = {};
            aes_setkey_enc(&AES, MetaInfo.Key, 128);
            aes_crypt_ctr(
                &AES,
                HeaderSize,
                &Offset,
                MetaInfo.IV,
                Stream,
                Headers,
                Headers
            );

            MetaHead = *(MetaData::Header*)Headers;

            DataKeysLength = MetaHead.KeyCount * 16;

            for(U32 I = 0; I < MetaHead.SectionCount; I++)
            {
                auto Section = *(MetaData::Section*)(Headers + sizeof(MetaData::Header) + sizeof(MetaData::Section) * I);

                if(Section.Encrypted == 3)
                    DataLength += Section.Size;

                MetaSections.push_back(Section);
            }

            DataKeys = Headers + sizeof(MetaData::Header) + MetaHead.SectionCount * sizeof(MetaData::Section);

            return true;
        }

        void Decrypt()
        {
            aes_context AES;

            U32 BufferOffset = 0;

            DataBuffer = new Byte[DataLength];

            for(auto Section : MetaSections)
            {
                if(Section.Encrypted == 3)
                {
                    size_t Offset = 0;

                    Byte Key[16];
                    Byte IV[16];

                    Memory::Copy(DataKeys + Section.KeyIndex * 16, Key, 16);
                    Memory::Copy(DataKeys + Section.IVIndex * 16, IV, 16);

                    Byte* Buffer = new Byte[Section.Size];

                    File.Seek(Section.Offset);
                    File.ReadN(Buffer, Section.Size);

                    Byte Stream[16] = {};

                    aes_setkey_enc(&AES, Key, 128);
                    aes_crypt_ctr(
                        &AES,
                        Section.Size,
                        &Offset,
                        IV,
                        Stream,
                        Buffer,
                        Buffer
                    );

                    Memory::Copy(Buffer, DataBuffer + BufferOffset, Section.Size);

                    BufferOffset += Section.Size;

                    delete[] Buffer;
                }
            }
        }

        Cthulhu::Binary ToELF()
        {
            Cthulhu::Binary Bin;
            return {};
        }
    };

    Cthulhu::Binary LoadSELF(FS::BufferedFile&& File, Byte* Key)
    {
        Decryptor Dec = File;

        if(!Dec.ReadHeaders())
        {
            VERROR("Failed to read EBOOT headers");
            return {};
        }

        if(!Dec.ReadMetadata(Key))
        {
            VERROR("Failed to read metadata");
            return {};
        }

        Dec.Decrypt();

        return Dec.ToELF();
    }   
}