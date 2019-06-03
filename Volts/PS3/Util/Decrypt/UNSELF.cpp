#include "UNSELF.h"

#include "Core/Logger/Logger.h"
#include "Core/Macros.h"

#include "Core/Convert.h"
#include "Keys.h"
#include "AES/aes.h"
#include "Core/AES/AES.h"

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
        struct Header
        {
            U32 Magic;

            U8 Class;
            U8 Endian;
            U8 SVersion;
            U8 ABI;
            U8 ABIVersion;
            Pad Padding[7];

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
#if 0
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
            if(Arch64)
            {
                delete[] Section64Headers;
                delete[] Program64Headers;
            }
            else
            {
                delete[] Section32Headers;
                delete[] Program32Headers;
            }

            delete[] DataBuffer;
            delete[] Headers;
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
        // this is safe to delete as no pointer arithmatic is performed on it
        Byte* Headers;
        // this isnt safe to delete, but will be deleted when Headers is deleted
        // as they point to the same data
        Byte* DataKeys;
        U32 DataBufferLength = 0;
        Byte* DataBuffer;


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
            SELF::ControlInfo Info;

            Info.Type = File.Read<Big<U32>>();
            Info.Size = File.Read<Big<U32>>();
            Info.Next = File.Read<Big<U64>>();

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

            // read in the app info
            File.Seek(SELFHeader.AInfoOffset);
            auto AInfo = File.Read<AppInfo>();

            // save the data we need
            SELFType = AInfo.Type;
            SELFVersion = AInfo.Version;

            // find the ELF header
            File.Seek(SELFHeader.ELFOffset);
            SmallELF = File.Read<ELF::SmallHeader>();

            if(SmallELF.Magic != "\177ELF"_U32)
            {
                LOG_ERROR(UNSELF, "Invalid ELF magic");
                return false;
            }

            // check if this is a 32 or 64 bit executable
            Arch64 = SmallELF.Class == 2;

            // 64 bit elf headers are a different size from 32 bit elf headers
            // so we need to read in the correct size
            if((Arch64 = SmallELF.Class == 2))
                BigELF64 = File.Read<ELF::BigHeader<U64>>();
            else
                BigELF32 = File.Read<ELF::BigHeader<U32>>();

            if(Arch64)
            {
                File.Seek(SELFHeader.SHeaderOffset);
                Section64Headers = new ELF::SectionHeader<U64>[BigELF64.SHCount];
                for(U32 I = 0; I < BigELF64.SHCount; I++)
                {
                    Section64Headers[I] = File.Read<ELF::SectionHeader<U64>>();
                }

                File.Seek(SELFHeader.PHeaderOffset);
                Program64Headers = new ELF::ProgramHeader<U64>[BigELF64.PHCount];
                for(U32 I = 0; I < BigELF64.PHCount; I++)
                {
                    Program64Headers[I] = File.Read<ELF::ProgramHeader<U64>>();
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

            const U32 HeaderSize = MetadataEnd - sizeof(SCE::Header) - MetadataStart - sizeof(MetaData::Info);
            Headers = new Byte[HeaderSize];
            File.ReadN(Headers, HeaderSize);

            aes_context AES;
            SELF::Key MetaKey = GetSELFKey(static_cast<KeyType>(SELFType), SCEFlags, SELFVersion);

            if((SCEFlags & 0x8000) != 0x8000)
            {
                if(!DecryptNPDRM((Byte*)&MetaInfo, sizeof(MetaData::Info), Key))
                    return false;

                aes_setkey_dec(&AES, MetaKey.ERK, 256);
                aes_crypt_cbc(&AES, AES_DECRYPT, sizeof(MetaData::Info), MetaKey.RIV, (Byte*)&MetaInfo, (Byte*)&MetaInfo);
            }

            for(U32 I = 0; I < sizeof(MetaInfo.Key); I++)
            {
                if(MetaInfo.KeyPad[I] | MetaInfo.IVPad[I])
                {
                    LOG_ERROR(UNSELF, "Failed to decrypt metadata info");
                    return false;
                }
            }

            size_t Offset = 0;
            Byte Stream[16];
            aes_setkey_enc(&AES, MetaInfo.Key, 128);
            aes_crypt_ctr(&AES, HeaderSize, &Offset, MetaInfo.IV, Stream, Headers, Headers);

            const auto MetaHead = *(MetaData::Header*)Headers;

            DataBufferLength = MetaHead.KeyCount * 16;

            for(U32 I = 0; I < MetaHead.SectionCount; I++)
            {
                auto Section = *(MetaData::Section*)(Headers + sizeof(MetaData::Header) + sizeof(MetaData::Section) * I);

                printf("%llu\n", Section.Offset.Get());

                if(Section.Encrypted == 3)
                {
                    DataBufferLength += Section.Size;
                }

                MetaSections.Append(Section);
            }

            DataKeys = Headers + sizeof(MetaData::Header) + MetaHead.SectionCount * sizeof(MetaData::Section);

            return true;
        }

        void DecryptData()
        {
            aes_context AES;

            U32 BufferOffset = 0;

            DataBuffer = new Byte[DataBufferLength];

            for(auto Section : MetaSections)
            {
                Byte Key[16];
                Byte IV[16];
                Byte Stream[16];
                size_t Offset = 0;

                if(Section.Encrypted == 3)
                {
                    Memory::Copy(DataKeys + Section.KeyIndex * 16, Key, 16);
                    Memory::Copy(DataKeys + Section.IVIndex * 16, IV, 16);

                    Byte* Buffer = new Byte[Section.Size];

                    File.Seek(Section.Offset);
                    File.ReadN(Buffer, Section.Size);

                    Memory::Zero(Stream, 16);

                    aes_setkey_enc(&AES, Key, 128);
                    aes_crypt_ctr(&AES, Section.Size, &Offset, IV, Stream, Buffer, Buffer);

                    Memory::Copy(Buffer, DataBuffer + BufferOffset, Section.Size);

                    BufferOffset += Section.Size;

                    delete[] Buffer;
                }
            }
        }

        template<typename TSections, typename THeader, typename TPrograms>
        ELF::Binary WriteELF(THeader* Header, TSections* Sections, TPrograms* Programs)
        {
            ELF::Binary Bin = {128};

            Bin.Write(&SmallELF);
            Bin.Write(Header);

            Bin.Seek(Header->PHOffset);
            for(U32 I = 0; I < Header->PHCount; I++)
            {
                Bin.Write(&Programs[I]);
            }

            U32 Offset = 0;

            printf("%u", MetaSections.Len());
            for(auto& Section : MetaSections)
            {
                printf("%u\n", Section.Type.Get());
                if(Section.Type == 2)
                {
                    Bin.Seek(Programs[Section.Index].Offset);

                    if(Section.Compressed == 3)
                    {
                        const U32 FileSize = Programs[Section.Index].FileSize;
                        Byte* DBuffer = new Byte[FileSize];
                        Byte* ZBuffer = new Byte[DataBufferLength];
                        Memory::Copy(DataBuffer, ZBuffer, DataBufferLength);

                        uLongf ZSize = static_cast<uLongf>(FileSize);

                        uncompress(DBuffer, &ZSize, ZBuffer + Offset, DataBufferLength);
                        Bin.Write(DBuffer, FileSize);

                        delete[] DBuffer;
                        delete[] ZBuffer;
                    }
                    else
                    {
                        Bin.Write(DataBuffer + Offset, Section.Size);

                        for(U32 L = 0; L < 25; L++)
                        {
                            printf("%u", (DataBuffer + Offset)[L]);
                        }
                    }

                    Offset += Section.Size;
                }
            }

            Bin.Seek(Header->SHOffset);
            for(U32 I = 0; I < Header->SHCount; I++)
            {
                Bin.Write(&Sections[I]);
            }

            Bin.Seek(0);
            return Bin;
        }

    public:
        ELF::Binary MakeELF()
        {
            if(Arch64)
                return WriteELF(&BigELF64, Section64Headers, Program64Headers);
            else
                return WriteELF(&BigELF32, Section32Headers, Program32Headers);
        }
    };
#endif

    struct FirmwareDecryptor
    {
    private:
        U64 HeaderSize;
        U64 MetaOffset;
        U16 Flags;

        MetaData::Section* Sections;
        U32 SectionCount;

        U32 KeyLength;
        Byte* Keys;

        U32 BufferLength;
        Byte* Buffer;

        Binary* Bin;

    public:
        FirmwareDecryptor(Binary* B)
            : Bin(B)
        {}

        bool LoadHeader()
        {
            auto SCE = Bin->Read<SCE::Header>();

            if(SCE.Magic != "SCE\0"_U32)
            {
                LOG_ERROR(FIRMWARE, "Invalid SCE Magic");
                return false;
            }

            HeaderSize = SCE.MetadataEnd - sizeof(SCE::Header) - SCE.MetadataStart - sizeof(MetaData::Info);
            MetaOffset = SCE.MetadataStart;
            Flags = SCE.Type;

            return true;
        }

        bool LoadMetadata()
        {
            Byte Info[sizeof(MetaData::Info)];
            Byte* Headers = new Byte[HeaderSize];

            Bin->Seek(MetaOffset);
            Bin->ReadN(Info, sizeof(MetaData::Info));

            Bin->ReadN(Headers, HeaderSize);

            Byte Key[32];
            Byte IV[16];

            Memory::Copy(Keys::SCEPKGERK, Key, 32);
            Memory::Copy(Keys::SCEPKGRIV, IV, 16);

            aes_context AES;

            if((Flags & 0x8000) != 0x8000)
            {
                aes_setkey_dec(&AES, Key, 256);
                aes_crypt_cbc(&AES, AES_DECRYPT, sizeof(MetaData::Info), IV, Info, Info);
            }

            auto MetaInfo = *(MetaData::Info*)Info;

            if(MetaInfo.IVPad[0] | MetaInfo.KeyPad[0])
            {
                LOG_ERROR(FIRMWARE, "Failed to decrypt Firmware");
                return false;
            }

            size_t Offset = 0;
            Byte Stream[16];

            aes_setkey_enc(&AES, MetaInfo.Key, 128);
            aes_crypt_ctr(&AES, HeaderSize, &Offset, MetaInfo.IV, Stream, Headers, Headers);

            auto MetaHead = *(MetaData::Header*)Headers;

            SectionCount = MetaHead.SectionCount;

            Sections = new MetaData::Section[SectionCount];

            BufferLength = 0;

            for(U32 I = 0; I < SectionCount; I++)
            {
                auto Sect = *(MetaData::Section*)(Headers + sizeof(MetaData::Header) + sizeof(MetaData::Section) * I);

                BufferLength += Sect.Size;

                Sections[I] = Sect;
            }

            KeyLength = MetaHead.KeyCount * 16;
            Keys = new Byte[KeyLength];
            Memory::Copy(Headers, Keys + sizeof(MetaData::Header) + SectionCount * sizeof(MetaData::Section), KeyLength);

            return true;
        }

        void DecryptData()
        {
            Buffer = new Byte[BufferLength];

            U32 BufferOffset = 0;
            aes_context AES;

            for(U32 I = 0; I < SectionCount; I++)
            {
                if(Sections[I].Encrypted == 3)
                {
                    size_t Offset = 0;
                    Byte Stream[16] = {};
                    Byte Key[16];
                    Byte IV[16];

                    Memory::Copy(Keys + Sections[I].KeyIndex * 16, Key, 16);
                    Memory::Copy(Keys + Sections[I].IVIndex * 16, IV, 16);

                    Byte* Temp = new Byte[Sections[I].Size];

                    Bin->Seek(Sections[I].Offset);
                    Bin->ReadN(Temp, Sections[I].Size);

                    aes_setkey_enc(&AES, Key, 128);
                    aes_crypt_ctr(&AES, Sections[I].Size, &Offset, IV, Stream, Temp, Temp);

                    Memory::Copy(Temp, Buffer + BufferOffset, Sections[I].Size);

                    delete[] Temp;
                }
                else
                {
                    Bin->Seek(Sections[I].Offset);
                    Bin->ReadN(Buffer + BufferOffset, Sections[I].Size);
                }

                BufferOffset += Sections[I].Size;
            }
        }

        Array<ELF::Binary> MakeELF()
        {
            Array<ELF::Binary> Arr;

            U32 Offset = 0;

            for(U32 I = 0; I < SectionCount; I++)
            {
                bool Valid = true;

                ELF::Binary Out;

                if(Sections[I].Compressed == 2)
                {
                    Byte Temp[32 * 1024];
                    z_stream Stream;
                    Stream.zalloc = Z_NULL;
                    Stream.zfree = Z_NULL;
                    Stream.opaque = Z_NULL;
                    Stream.avail_in = Sections[I].Size;
                    Stream.avail_out = 32 * 1024;
                    Stream.next_in = Buffer + Offset;
                    Stream.next_out = Temp;
                    int Ret = inflateInit(&Stream);

                    while(Stream.avail_in)
                    {
                        Ret = inflate(&Stream, Z_NO_FLUSH);

                        if(Ret == Z_STREAM_END)
                            break;

                        if(Ret != Z_OK)
                            Valid = false;

                        if(!Stream.avail_out)
                        {
                            Out.Write(Temp, 32 * 1024);
                            Stream.next_out = Temp;
                            Stream.avail_out = 32 * 1024;
                        }
                        else
                        {
                            break;
                        }
                    }

                    int Res = inflate(&Stream, Z_FINISH);

                    if(Res != Z_STREAM_END)
                        Valid = false;

                    Out.Write(Temp, 32 * 1024 - Stream.avail_out);
                    inflateEnd(&Stream);
                }
                else
                {
                    Out.Write(Buffer + Offset, Sections[I].Size);
                }

                Offset += Sections[I].Size;

                if(Valid)
                    Arr.Append(Out);
            }

            return Arr;
        }
    };

    class SELFDecryptor
    {
        bool Arch64;
        FileSystem::BufferedFile* File;
        SCE::Header SCEHead;
        SELF::Header SELFHead;
        AppInfo AInfo;

        ELF::Header ELFHead;
        Array<ELF::ProgramHeader> PHead;
        Array<ELF::SectionHeader> SHead;
        Array<SELF::ControlInfo> Controls;

        SELF::ControlInfo ReadControlInfo()
        {
            SELF::ControlInfo Info;

            Info.Type = File->Read<Big<U32>>();
            Info.Size = File->Read<Big<U32>>();
            Info.Next = File->Read<Big<U64>>();

            if(Info.Type == 1)
            {
                Info.ControlFlags = File->Read<decltype(SELF::ControlInfo::ControlFlags)>();
            }
            else if(Info.Type == 2 && Info.Size == 48)
            {
                Info.ELFDigest40 = File->Read<decltype(SELF::ControlInfo::ELFDigest40)>();
            }
            else if(Info.Type == 2 && Info.Size == 64)
            {
                Info.ELFDigest30 = File->Read<decltype(SELF::ControlInfo::ELFDigest30)>();
            }
            else if(Info.Type == 3)
            {
                Info.NPDRMInfo = File->Read<decltype(SELF::ControlInfo::NPDRMInfo)>();
            }
            else
            {
                LOGF_ERROR(UNSELF, "Invalid control type of %u", Info.Type);
            }

            return Info;
        }

    public:

        SELFDecryptor(FileSystem::BufferedFile* F)
            : File(F)
        {
            File->Seek(0);
        }

        bool ReadHeaders()
        {
            SCEHead = File->Read<SCE::Header>();

            LOGF_DEBUG(UNSELF,
                "SCE magic %u version %u type %u category %u metastart %u metaend %llu size %llu",
                SCEHead.Magic,
                SCEHead.Version.Get(),
                SCEHead.Type.Get(),
                SCEHead.Category.Get(),
                SCEHead.MetadataStart.Get(),
                SCEHead.MetadataEnd.Get(),
                SCEHead.Size.Get()
            );

            if(SCEHead.Magic != "SCE\0"_U32)
            {
                LOG_ERROR(UNSELF, "Invalid SCE magic");
                return false;
            }

            SELFHead = File->Read<SELF::Header>();
            LOGF_DEBUG(UNSELF,
                "SELF type %llu ainfo %llu elfoff %llu phoff %llu shoff %llu sinfo %llu voff %llu coff %llu clen %llu",
                SELFHead.Type.Get(),
                SELFHead.AInfoOffset.Get(),
                SELFHead.ELFOffset.Get(),
                SELFHead.PHeaderOffset.Get(),
                SELFHead.SHeaderOffset.Get(),
                SELFHead.SInfoOffset.Get(),
                SELFHead.VersionOffset.Get(),
                SELFHead.ControlOffset.Get(),
                SELFHead.ControlLength.Get()
            );

            AInfo = File->Read<AppInfo>();

            LOGF_DEBUG(UNSELF,
                "AppInfo auth %llu vendor %u type %u version %llu",
                AInfo.AuthID.Get(),
                AInfo.VendorID.Get(),
                AInfo.Type.Get(),
                AInfo.Version.Get()
            );

            ELFHead = File->Read<ELF::Header>();

            if(ELFHead.Magic != "\177ELF"_U32)
            {
                LOG_ERROR(UNSELF, "Invalid ELF magic");
                return false;
            }

            LOGF_DEBUG(UNSELF,
                "ELF magic %u class %u endian %u sversion %u abi %u abiver %u type %u machine %u version %u entry %llu phoff %llu shoff %llu flags %u hsize %u phsize %u phcount %u shsize %u shcount %u str %u",
                ELFHead.Magic,
                ELFHead.Class,
                ELFHead.Endian,
                ELFHead.SVersion,
                ELFHead.ABI,
                ELFHead.ABIVersion,
                ELFHead.Type.Get(),
                ELFHead.Machine.Get(),
                ELFHead.Version.Get(),
                ELFHead.Entry.Get(),
                ELFHead.PHOffset.Get(),
                ELFHead.SHOffset.Get(),
                ELFHead.Flags.Get(),
                ELFHead.HeaderSize.Get(),
                ELFHead.PHEntrySize.Get(),
                ELFHead.PHCount.Get(),
                ELFHead.SHEntrySize.Get(),
                ELFHead.SHCount.Get(),
                ELFHead.StringIndex.Get()
            );

            File->Seek(SELFHead.SHeaderOffset);

            for(U32 I = 0; I < ELFHead.SHCount; I++)
            {
                auto Section = File->Read<ELF::SectionHeader>();

                LOGF_DEBUG(UNSELF,
                    "Section str %u type %u flags %llu virt %llu off %llu size %llu link %u info %u align %llu entry %llu",
                    Section.StringOffset.Get(),
                    Section.Type.Get(),
                    Section.Flags.Get(),
                    Section.VirtualAddress.Get(),
                    Section.Offset.Get(),
                    Section.Size.Get(),
                    Section.Link.Get(),
                    Section.Info.Get(),
                    Section.Align.Get(),
                    Section.EntrySize.Get()
                );

                SHead.Append(Section);
            }

            File->Seek(SELFHead.PHeaderOffset);

            for(U32 I = 0; I < ELFHead.PHCount; I++)
            {
                auto Program = File->Read<ELF::ProgramHeader>();

                LOGF_DEBUG(UNSELF,
                    "Program type %u flags %u offset %llu virt %llu phys %llu fsize %llu memsize %llu align %llu",
                    Program.Type.Get(),
                    Program.Flags.Get(),
                    Program.Offset.Get(),
                    Program.VirtualAddress.Get(),
                    Program.PhysicalAddress.Get(),
                    Program.FileSize.Get(),
                    Program.MemorySize.Get(),
                    Program.Align.Get()
                );

                PHead.Append(Program);
            }

            LOGF_DEBUG(UNSELF, "Depth %u", File->CurrentDepth());
            File->Seek(SELFHead.ControlOffset);
            LOGF_DEBUG(UNSELF, "Depth %u", File->CurrentDepth());

            U32 I = 0;
            while(I < SELFHead.ControlLength)
            {
                auto Info = ReadControlInfo();
                I += Info.Size;

                LOGF_DEBUG(UNSELF,
                    "Control type %u size %u next %llu",
                    Info.Type,
                    Info.Size,
                    Info.Next
                );

                Controls.Append(Info);
            }

            return true;
        }

        bool ReadMetadata(Byte* Key)
        {
            return true;
        }

        void DecryptData()
        {

        }

    private:

        template<typename THeader, typename TSection, typename TProgram>
        ELF::Binary CreateBinary(THeader* Header, TSection* Sections, TProgram* TPrograms)
        {
            ELF::Binary Bin = {};

            return Bin;
        }

    public:

        ELF::Binary MakeELF()
        {
            if(Arch64);
                //return CreateBinary();
            else;
                //return CreateBinary();
                return {};
        }
    };

    namespace UNSELF
    {
        Cthulhu::Option<ELF::Binary> DecryptSELF(Cthulhu::FileSystem::BufferedFile& File, Byte* Key)
        {
            SELFDecryptor Decrypt(&File);

            if(!Decrypt.ReadHeaders())
            {
                LOG_ERROR(UNSELF, "Failed to read headers");
                return None<ELF::Binary>();
            }

            if(!Decrypt.ReadMetadata(Key))
            {
                LOG_ERROR(UNSELF, "Failed to read metadata");
                return None<ELF::Binary>();
            }

            Decrypt.DecryptData();

            return Some(Decrypt.MakeELF());
        }

        Cthulhu::Array<ELF::Binary> DecryptFirmware(Cthulhu::Binary& Bin)
        {
            FirmwareDecryptor Decrypt(&Bin);

            if(!Decrypt.LoadHeader())
            {
                return {};
            }

            if(!Decrypt.LoadMetadata())
            {
                return {};
            }

            Decrypt.DecryptData();

            return Decrypt.MakeELF();
        }
    }
}
