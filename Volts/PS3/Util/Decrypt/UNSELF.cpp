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

    // used for decrypting SELF files
    // functions should always be called in the correct order
    // that being
    // 1. ReadHeaders() to read in unencrypted stuff
    // 2. ReadMetadata() to read in semi encrypted stuff (can be slow)
    // 3. DecryptData() to decrypt all the data we've got (this is really slow)
    // 4. MakeELF() to convert the decrypted data into an ELF file we can work with easily
    class SELFDecryptor
    {
        FileSystem::BufferedFile* File;
        SCE::Header SCEHead;
        SELF::Header SELFHead;
        AppInfo AInfo;

        ELF::Header ELFHead;
        Array<ELF::ProgramHeader> PHead;
        Array<ELF::SectionHeader> SHead;
        Array<SELF::ControlInfo> Controls;
        Array<MetaData::Section> MetaSections;

        U32 DataKeysLength;
        Byte* DataKeys;

        U32 DataLength = 0;
        Byte* DataBuffer;

        // read in a SELF control info section
        SELF::ControlInfo ReadControlInfo()
        {
            // create struct for later
            SELF::ControlInfo Info;

            // read in the data that will always be there
            Info.Type = File->Read<Big<U32>>();
            Info.Size = File->Read<Big<U32>>();
            Info.Next = File->Read<Big<U64>>();

            // the size of the struct varies depending on the type of the struct
            // so we have to read in different amounts
            if(Info.Type == 1)
            {
                // we read in using decltype(T) rather than a raw typename
                // because these are all anonymous structs that cannot be referenced
                // by name as they have none, so we get their type by getting the type
                // of the variable
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
                // something went wrong so we have to log that
                LOGF_ERROR(UNSELF, "Invalid control type of %u", Info.Type);
            }

            // return the data we've read in
            return Info;
        }

        bool DecryptNPDRM(Byte* Metadata, U32 Len, Byte* MetaKey)
        {
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

            Byte Key[16];
            Byte IV[16];

            if(Control->NPDRMInfo.Version == 1)
            {
                LOG_ERROR(UNSELF, "Cannot decrypt network license");
                return false;
            }
            else if(Control->NPDRMInfo.Version == 2)
            {
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

            return true;
        }

    public:

        // create a new decryptor
        // F is the file we need to decrypt
        SELFDecryptor(FileSystem::BufferedFile* F)
            : File(F)
        {
            // go to the front of the file to make sure we start at the correct offset
            File->Seek(0);
        }

        ~SELFDecryptor()
        {
            // TODO: this leaks but for some reason the delete[] crashes on windows
            //delete[] Data;
            //delete[] DataBuffer;
        }

        // read in the files unencrypted headers
        // returns true when everything goes well
        // otherwise returns false and will log what went wrong (bad magic numbers)
        bool ReadHeaders()
        {
            // read in the SCE header, this is at the very front of the file
            // this contains some of the data needed to parse the file
            // but not all of it
            SCEHead = File->Read<SCE::Header>();

            // make sure we have valid magic
            // this just lets us know we've been fed the right type of file
            if(SCEHead.Magic != "SCE\0"_U32)
            {
                // if we havent spit out an error
                LOG_ERROR(UNSELF, "Invalid SCE magic");
                return false;
            }

            // next comes the self header
            // this contains more data about the file
            // specifically data about the ELF executable
            // including Section and Program header offsets
            SELFHead = File->Read<SELF::Header>();

            // read in AppInfo
            // some of the data in this struct is used to
            // grab the correct key to decrypt the files contents
            AInfo = File->Read<AppInfo>();

            // read in the ELF header
            // unlike a usual ELF parser we only need 64 bit structures
            // this is because all SELF files have 64 bit ELFs in them
            ELFHead = File->Read<ELF::Header>();

            // sanity check the magic just in case something went tits up
            if(ELFHead.Magic != "\177ELF"_U32)
            {
                // if something went wrong spit out the file
                LOG_ERROR(UNSELF, "Invalid ELF magic");
                return false;
            }

            // next come the ELF program headers, these are right after the ELF header
            for(U32 I = 0; I < ELFHead.PHCount; I++)
            {
                // for every Program header we read it in
                // then add it to our list of program headers
                PHead.Append(File->Read<ELF::ProgramHeader>());
            }

            // next we need the control info structs
            // these also have data we need to decrypt the data
            File->Seek(SELFHead.ControlOffset);

            // ControlInfo structs can be multiple sizes
            // so we store the total length of structs we read in I
            // then add the length of each to struct to I until we've
            // read as much as the header says there is
            U32 I = 0;
            while(I < SELFHead.ControlLength)
            {
                // so we read in the control info
                auto Info = ReadControlInfo();
                // then add the size we've read
                I += Info.Size;

                // then add the control to out total controls
                Controls.Append(Info);
            }


            // now we seek to the Section header start offset
            File->Seek(SELFHead.SHeaderOffset);

            // then for every ELF section header
            for(U32 I = 0; I < ELFHead.SHCount; I++)
            {
                // we read in the header
                // then add it to our list of section headers
                SHead.Append(File->Read<ELF::SectionHeader>());
            }


            return true;
        }

        // read in the encrypted metadata and decrypt it
        bool ReadMetadata(Byte* Key)
        {
            // find the start of the metadata block
            File->Seek(SCEHead.MetadataStart + sizeof(SCE::Header));

            // then read in the metadata info section, this contains stuff we'll need
            // late to decrypt more stuff
            auto MetaInfo = File->Read<MetaData::Info>();

            // allocate space for the headers
            const U32 HeaderSize = SCEHead.MetadataEnd - sizeof(SCE::Header) - SCEHead.MetadataStart - sizeof(MetaData::Info);
            Byte* Headers = new Byte[HeaderSize];

            // so we do need to seek here to prevent some funky off by one errors
            File->Seek(SCEHead.MetadataStart + sizeof(SCE::Header) + sizeof(MetaData::Info));
            // then read in the raw sections to decrypt
            File->ReadN(Headers, HeaderSize);

            aes_context AES;
            // find the self key we need to decrypt this data
            SELF::Key MetaKey = GetSELFKey((KeyType)AInfo.Type.Get(), SCEHead.Type, AInfo.Version);

            // if the SELF isnt a debug self we'll need to decrypt some stuff
            if((SCEHead.Type & 0x8000) != 0x8000)
            {
                // if that decryption fails just give up
                if(!DecryptNPDRM((Byte*)&MetaInfo, sizeof(MetaData::Info), Key))
                    return false;

                // otherwise decrypt the metadata info section
                aes_setkey_dec(&AES, MetaKey.ERK, 256);
                aes_crypt_cbc(&AES, AES_DECRYPT, sizeof(MetaData::Info), MetaKey.RIV, (Byte*)&MetaInfo, (Byte*)&MetaInfo);
            }

            // the padding should always be 0
            // so we can make sure it is 0 to be sure we actually decrypted the data
            for(U32 I = 0; I < 16; I++)
            {
                // | is faster than a branch so just do that
                if(MetaInfo.KeyPad[I] | MetaInfo.IVPad[I])
                {
                    // if something isnt 0 (false) then we faild decryption
                    // report it and back out
                    LOG_ERROR(UNSELF, "Failed to decrypt metadata info");
                    return false;
                }
            }

            // now we decrypt the metadata section headers
            size_t Offset = 0;
            Byte Stream[16];
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

            // the first bit of data is the section header, so grab that
            const auto MetaHead = *(MetaData::Header*)Headers;

            // we need data length for decrypting the body
            DataKeysLength = MetaHead.KeyCount * 16;

            // then for every section past the header
            for(U32 I = 0; I < MetaHead.SectionCount; I++)
            {
                // convert the raw data to the struct
                auto Section = *(MetaData::Section*)(Headers + sizeof(MetaData::Header) + sizeof(MetaData::Section) * I);

                // if the section body is encrypted well need to decrypt it
                // so we add to the datalength so we know the size later
                if(Section.Encrypted == 3)
                    DataLength += Section.Size;

                // then add it to our metadata sections
                MetaSections.Append(Section);
            }

            // store the data for later so we can get keys and vectors out of it
            DataKeys = Headers + sizeof(MetaData::Header) + MetaHead.SectionCount * sizeof(MetaData::Section);

            // everything worked properly
            return true;
        }

        // actually decrypt the data we've collected
        void DecryptData()
        {
            // get an aes context to decrypt with
            // this can be reused so no point reallocating it each loop
            aes_context AES;

            U32 BufferOffset = 0;
            // create that buffer we got the length for earlier
            DataBuffer = new Byte[DataLength];

            Byte Key[16];
            Byte IV[16];

            // for all the meta sections
            for(auto Section : MetaSections)
            {
                // if the section is encrypted
                if(Section.Encrypted == 3)
                {
                    size_t Offset = 0;
                    // get the key and init vector from the decrypted sections
                    Memory::Copy(DataKeys + Section.KeyIndex * 16, Key, 16);
                    Memory::Copy(DataKeys + Section.IVIndex * 16, IV, 16);

                    // then allocate a buffer for decryption into
                    Byte* Buffer = new Byte[Section.Size];

                    // then go actually fetch the data
                    File->Seek(Section.Offset);
                    File->ReadN(Buffer, Section.Size);

                    Byte Stream[16] = {};
                    Memory::Zero(Stream, 16);

                    printf("Len %llu\n", Section.Size.Get());

                    printf("Enc: ");
                    for(U32 I = 0; I < 25; I++)
                    {
                        printf("%u ", Buffer[I]);
                    }
                    printf("\n");

                    printf("Key: ");
                    for(auto A : Key)
                    {
                        printf("%u ", A);
                    }
                    printf("\n");

                    printf("IV: ");
                    for(auto A : IV)
                    {
                        printf("%u ", A);
                    }
                    printf("\n");

                    printf("Stream: ");
                    for(auto A : Stream)
                    {
                        printf("%u ", A);
                    }
                    printf("\n");

                    printf("NCOff %zu\n", Offset);

                    // once we have the data we can then decrypt it using AES 128 CTR
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

                    printf("Dec: ");
                    for(U32 I = 0; I < 25; I++)
                    {
                        printf("%u ", Buffer[I]);
                    }
                    printf("\n");

                    // then copy the decrypted data into out buffer
                    Memory::Copy(Buffer, DataBuffer + BufferOffset, Section.Size);

                    // then add to the buffer offset so we know where to seek next time
                    BufferOffset += Section.Size;

                    // and clean up after ourselves
                    delete[] Buffer;
                }
            }
        }

    public:

        // turns all our data into an ELF binary we can use
        ELF::Binary MakeELF()
        {
            auto* KF = fopen("out.elf", "w");
            I64 K = 0;
            ELF::Binary Bin = {128};

            fwrite(&ELFHead, sizeof(ELFHead), 1, KF);

            // write the elf header
            Bin.Write(&ELFHead);

            fseek(KF, ELFHead.PHOffset, SEEK_SET);

            // now we write the ELF program headers
            Bin.Seek(ELFHead.PHOffset);
            for(auto Program : PHead)
            {
                Bin.Write(&Program);
                fwrite(&Program, sizeof(Program), 1, KF);
            }

            U32 BufferOffset = 0;

            // for every metadata section
            for(auto& Section : MetaSections)
            {
                // if its a program header
                if(Section.Type == 2)
                {
                    fseek(KF, PHead[Section.Index].Offset, SEEK_SET);
                    Bin.Seek(PHead[Section.Index].Offset);
                    LOGF_DEBUG(UNSELF, "Seek %llu", PHead[Section.Index].Offset.Get());
                    // if the section is compressed
                    if(Section.Compressed == 3)
                    {
                        // decompress it
                        const U32 FileSize = PHead[Section.Index].FileSize;
                        Byte* DBuffer = new Byte[FileSize];
                        Byte* ZBuffer = new Byte[DataLength];

                        Memory::Copy(DataBuffer, ZBuffer, DataLength);

                        uLongf ZSize = static_cast<uLongf>(FileSize);

                        uncompress(DBuffer, &ZSize, ZBuffer + BufferOffset, DataLength);

                        // then write it to the file
                        Bin.Write(DBuffer, FileSize);

                        fwrite(DBuffer, sizeof(Byte), FileSize, KF);

                        // be sure to clean up after ourselves
                        delete[] DBuffer;
                        delete[] ZBuffer;
                    }
                    else
                    {
                        for(U32 I = 0; I < 25ULL; I++)
                        {
                            //LOGF_DEBUG(UNSELF, "Z %u", (DataBuffer + BufferOffset)[I]);
                        }
                        // otherwise we can just write the data
                        Bin.Write(DataBuffer + BufferOffset, Section.Size);

                        fwrite(DataBuffer + BufferOffset, sizeof(Byte), Section.Size, KF);
                    }

                    BufferOffset += Section.Size;
                }
            }

            LOGF_DEBUG(UNSELF, "Depth %u", Bin.Depth());

            fseek(KF, ELFHead.SHOffset, SEEK_SET);

            // now we write the ELF section headers
            Bin.Seek(ELFHead.SHOffset);
            for(auto Section : SHead)
            {
                Bin.Write(&Section);
                fwrite(&Section, sizeof(Section), 1, KF);
            }

            LOGF_DEBUG(UNSELF, "Depth %u", Bin.Depth());

            // seek to the front so the output is clean
            Bin.Seek(0);
            fclose(KF);
            return Bin;
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
    }
}
