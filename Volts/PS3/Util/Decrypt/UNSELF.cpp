#include "UNSELF.h"

#include "Core/Logger/Logger.h"

#include "PS3/Util/Convert.h"
#include "Keys.h"
#include "AES/aes.h"

#include <zlib.h>

using namespace Cthulhu;

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
            Big<U32> MetadataOffset;
            Big<U64> HeaderLength;
            Big<U64> DataLength;
        };

        struct VersionInfo
        {
            Big<U32> SubType;
            Big<U32> Present;
            Big<U32> Size;
            U8 Padding[4];
        };
    }

    namespace ELF
    {
        struct SmallHeader
        {
            Big<Cthulhu::U32> Magic;
            Cthulhu::U8 Class;
            Cthulhu::U8 Endianness;
            Cthulhu::U8 Version;
            Cthulhu::U8 ABI;
            Cthulhu::U8 ABIVersion;
            Cthulhu::U8 Padding[7];
        };

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

        template<typename T>
        struct ProgramHeader {};

        template<> 
        struct ProgramHeader<Cthulhu::U32> 
        {
            Big<Cthulhu::U32> Type;
            Big<Cthulhu::U32> Flags;

            Big<Cthulhu::U32> Offset;
            Big<Cthulhu::U32> VirtualAddress;
            Big<Cthulhu::U32> PhysicalAddress;
            Big<Cthulhu::U32> FileSize;
            Big<Cthulhu::U32> MemorySize;

            Big<Cthulhu::U32> Align;
        };

        template<> 
        struct ProgramHeader<Cthulhu::U64> 
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
    }

    struct AppInfo
    {
        Big<U64> AuthID;
        Big<U32> VendorID;
        Big<KeyType> Type;
        Big<U32> Version;
        U8 Padding[12];
    };

    struct SectionInfo
    {
        Big<U64> Offset;
        Big<U64> Size;
        Big<U32> Compressed; // 1 if uncompressed, 2 if compressed
        U8 Padding[8]; // should always be 0
        Big<U32> Encrypted; // 1 if encrypted, 2 if unecrypted
    };

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

            struct
            {
                U8 Constant[20];
                U8 Digest[20];
                Big<U64> RequiredSystemVersion;
            } ELFDigest40;

            struct
            {
                U8 ConstantOrDigest[20];
                U8 Padding[12];
            } ELFDigest30;

            struct
            {
                Big<U32> Magic;
                Big<U32> LicenseVersion;
                Big<U32> DRMType;
                Big<U32> AppType;
                U8 ContentID[48];
                U8 Digest[16];
                U8 INVDigest[16];
                U8 XORDigest[16];
                U8 Padding[16];
            } NPDRMInfo;
        };
    };

    namespace MetaData
    {
        struct Header
        {
            Big<U64> SignatureLength;
            U32 Padding1;
            Big<U32> SectionCount;
            Big<U32> KeyCount;
            Big<U32> HeaderSize;
            U32 Padding2;
            U32 Padding3;
        };

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

        struct Info
        {
            U8 Key[16];
            U8 KeyPadding[16];

            U8 IV[16];
            U8 IVPadding[16];
        };
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

            //delete[] KeyData;
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
            Info = File.Read<AppInfo>();

            // read in a small elf header so we know how the rest of the header is formatted
            File.Seek(SELFHead.ELFOffset);
            auto Small = File.Read<ELF::SmallHeader>();

            // check the magic of the ELF small header
            if(Small.Magic != 0x7F454C46)
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
            }

            // seek to the start of the program headers
            File.Seek(SELFHead.ProgramHeaderOffset);

            // if the elf file is 32 bit
            if(Is32)
            {
                // load program headers and then section headers
                PHeaders32 = new Array<ELF::ProgramHeader<U32>>();

                if(ELFHead32.PHOffset == 0 && ELFHead32.PHCount)
                {
                    LOG_ERROR(UNSELF, "ELF Program header offset is 0");
                    return false;
                }

                for(U32 I = 0; I < ELFHead32.PHCount; I++)
                {
                    PHeaders32->Append(File.Read<ELF::ProgramHeader<U32>>());
                }

                SHeaders32 = new Array<ELF::SectionHeader<U32>>();

                if(ELFHead32.SHOffset == 0 && ELFHead32.SHCount)
                {
                    LOG_ERROR(UNSELF, "ELF Section header offset is 0");
                    return false;
                }

                File.Seek(SELFHead.SectionHeaderOffset);

                for(U32 I = 0; I < ELFHead32.SHCount; I++)
                {
                    SHeaders32->Append(File.Read<ELF::SectionHeader<U32>>());
                }
            }
            else 
            {
                PHeaders64 = new Array<ELF::ProgramHeader<U64>>();

                if(ELFHead64.PHOffset == 0 && ELFHead64.PHCount)
                {
                    LOG_ERROR(UNSELF, "ELF Program header offset is 0");
                    return false;
                }

                for(U32 I = 0; I < ELFHead64.PHCount; I++)
                {
                    PHeaders64->Append(File.Read<ELF::ProgramHeader<U64>>());
                }

                SHeaders64 = new Array<ELF::SectionHeader<U64>>();

                if(ELFHead64.SHOffset == 0 && ELFHead64.SHCount)
                {
                    LOG_ERROR(UNSELF, "ELF Section header offset is 0");
                    return false;
                }

                File.Seek(SELFHead.SectionHeaderOffset);

                for(U32 I = 0; I < ELFHead64.SHCount; I++)
                {
                    SHeaders64->Append(File.Read<ELF::SectionHeader<U64>>());
                }
            }
            
            File.Seek(SELFHead.VersionOffset);
            SCEVer = File.Read<decltype(SCEVer)>();
            
            // read control info
            File.Seek(SELFHead.ControlOffset);

            U32 I = 0;
            while(I < SELFHead.ControlLength)
            {
                auto CInfo = ReadControlInfo();
                CInfoArray.Append(CInfo);
                I += CInfo.Size;
            }

            // read in the section info headers
            File.Seek(SELFHead.SectionInfoOffset);
            const U32 InfoCount = (Is32 ? ELFHead32.PHCount : ELFHead64.PHCount);

            for(U32 I = 0; I < InfoCount; I++)
            {
                SInfoArray.Append(File.Read<SectionInfo>());
            }

            return true;
        }

    private:
        bool KeyFromRAP(U8* ID, U8* Key)
        {
            //U8 RAPKey[16] = {};
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
            File.Seek(SCEHead.MetadataOffset + sizeof(SCE::Header));
            auto MetaInfo = File.Read<MetaData::Info>();
            
            aes_context AES;
            
            SELF::Key MetaKey = GetSELFKey(Info.Type, SCEHead.KeyType, Info.Version);

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

            const U32 HSize = SCEHead.HeaderLength - (sizeof(SCE::Header) + SCEHead.MetadataOffset + sizeof(MetaData::Info));
            U8* Headers = new U8[HSize];

            File.Seek(SCEHead.MetadataOffset + sizeof(SCE::Header) + sizeof(MetaData::Info));
            File.ReadN(Headers, HSize);

            size_t Offset = 0;
            U8 Stream[16];
            aes_setkey_enc(&AES, MetaInfo.Key, 128);
            aes_crypt_ctr(&AES, HSize, &Offset, MetaInfo.IV, Stream, Headers, Headers);

            MetaData::Header MetaHead = *(MetaData::Header*)Headers;

            for(U32 I = 0; I < MetaHead.SectionCount; I++)
            {
                MetaSections.Append(*(MetaData::Section*)(Headers + sizeof(MetaData::Section) * I));
            }

            KeyData = Headers;

            return true;
        }

        void DecryptData() 
        { 
            aes_context AES;

            BufferLength = MetaHead.KeyCount * 16;

            for(auto& Section : MetaSections)
                if(Section.Encrypted == 3)
                    if((Section.KeyIndex <= MetaHead.KeyCount - 1) && (Section.IVIndex <= MetaHead.KeyCount))
                        BufferLength += Section.Size;
            
            DataBuffer = new U8[BufferLength];

            size_t Offset = 0;
            
            U8 Key[16];
            U8 IV[16];

            for(const auto& Section : MetaSections)
            {
                if(
                    (Section.Encrypted == 3) && 
                    (Section.KeyIndex <= MetaHead.KeyCount - 1) && 
                    (Section.IVIndex <= MetaHead.KeyCount)
                )
                {
                    U8 Stream[16] = {};
                    Memory::Copy<U8>(KeyData + Section.KeyIndex * 16, Key, 16);
                    Memory::Copy<U8>(KeyData + Section.IVIndex * 16, IV, 16);

                    File.Seek(Section.Offset);
                    
                    U8* Data = new U8[Section.Size];
                    File.ReadN(Data, Section.Size);

                    Memory::Zero<U8>(Stream, 16);

                    aes_setkey_enc(&AES, Key, 128);
                    aes_crypt_ctr(&AES, Section.Size, &Offset, IV, Stream, Data, Data);

                    Memory::Copy<U8>(DataBuffer + Offset, Data, Section.Size);

                    Offset += Section.Size;
                }
            }
        }

        ELF::Binary CreateBinary()
        {
            ELF::Binary Binary;

            //U32 Offset = 0;
            
            for(const auto& Sect : MetaSections)
            {
                LOG_DEBUG(UNSELF, "-----------------");
                LOGF_DEBUG(UNSELF, "Offset = %llu", Sect.Offset.Get());
                LOGF_DEBUG(UNSELF, "Size = %llu", Sect.Size.Get());
                LOGF_DEBUG(UNSELF, "Type = %u", Sect.Type.Get());
                LOGF_DEBUG(UNSELF, "ProgramIndex = %u", Sect.ProgramIndex.Get());
                LOGF_DEBUG(UNSELF, "HashAlgo = %u", Sect.HashAlgo.Get());
                LOGF_DEBUG(UNSELF, "HashIndex = %u", Sect.HashIndex.Get());
                LOGF_DEBUG(UNSELF, "Encrypted = %u", Sect.Encrypted.Get());
                LOGF_DEBUG(UNSELF, "KeyIndex = %u", Sect.KeyIndex.Get());
                LOGF_DEBUG(UNSELF, "IVIndex = %u", Sect.IVIndex.Get());
                LOGF_DEBUG(UNSELF, "Compressed = %u", Sect.Compressed.Get());
            }

            return Binary;
        }

        bool Is32;

        FS::BufferedFile& File;

        SCE::Header SCEHead;
        SELF::Header SELFHead;
        AppInfo Info;

        Array<SectionInfo> SInfoArray;
        Array<ControlInfo> CInfoArray;

        SCE::VersionInfo SCEVer;

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

        MetaData::Header MetaHead;
        MetaData::Info MetaInfo;
        Array<MetaData::Section> MetaSections;

        U8* DataBuffer;

        U32 BufferLength;
        U8* KeyData;
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

            //printf("Test\n");
            //Decrypt.Test();

            //return None<ELF::Binary>();
            return Some(Decrypt.CreateBinary());
        }
    }
}

