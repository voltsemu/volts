#include "UNSELF.h"

#include "Core/Logger/Logger.h"
#include "Core/Macros.h"

#include "Core/Convert.h"
#include "Keys.h"
#include "AES/aes.h"
#include "Core/AES/AES.h"

#include <zlib.h>

#include "Config.h"

namespace Volts::PS3
{
    using namespace Cthulhu;

    using Pad = Byte;
#if 0
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

        MetaData::Header MetaHead;

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
                VERROR("Invalid control type of %u", Info.Type);
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

            if(Control->NPDRMInfo.Version == 1)
            {
                VERROR("Cannot decrypt network license");
                return false;
            }
            else if(Control->NPDRMInfo.Version == 2)
            {
                VERROR("Local licenses are not supported yet");
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
                VERROR("Invalid SCE magic");
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
                VERROR("Invalid ELF magic");
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
            // later to decrypt more stuff
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
                    // if something isnt 0 (false) then we failed decryption
                    // report it and back out
                    VERROR("Failed to decrypt metadata info");
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
            MetaHead = *(MetaData::Header*)Headers;

            // we need data length for decrypting the body
            DataKeysLength = MetaHead.KeyCount * 16;

            // then for every section past the header
            for(U32 I = 0; I < MetaHead.SectionCount; I++)
            {
                // convert the raw data to the struct
                auto Section = *(MetaData::Section*)(Headers + sizeof(MetaData::Header) + sizeof(MetaData::Section) * I);

                // if the section body is encrypted we'll need to decrypt it
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

            // for all the meta sections
            for(auto Section : MetaSections)
            {
                // if the section is encrypted
                if(Section.Encrypted == 3)
                {
                    size_t Offset = 0;

                    Byte Key[16];
                    Byte IV[16];

                    // get the key and init vector from the decrypted sections
                    Memory::Copy(DataKeys + Section.KeyIndex * 16, Key, 16);
                    Memory::Copy(DataKeys + Section.IVIndex * 16, IV, 16);

                    // then allocate a buffer for decryption into
                    Byte* Buffer = new Byte[Section.Size];

                    // then go actually fetch the data
                    File->Seek(Section.Offset);
                    File->ReadN(Buffer, Section.Size);

                    Byte Stream[16] = {};

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
            ELF::Binary Bin;

            // write the elf header
            Bin.Write(ELFHead);

            // now we write the ELF program headers
            Bin.Seek(ELFHead.PHOffset);

            for(auto Program : PHead)
                Bin.Write(Program);

            U32 BufferOffset = 0;

            // for every metadata section
            for(auto& Section : MetaSections)
            {
                // if its a program header
                if(Section.Type == 2)
                {
                    Bin.Seek(PHead[Section.Index].Offset);
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
                        Bin.WriteN(DBuffer, FileSize);

                        // be sure to clean up after ourselves
                        delete[] DBuffer;
                        delete[] ZBuffer;
                    }
                    else
                    {
                        // otherwise we can just write the data
                        Bin.WriteN(DataBuffer + BufferOffset, Section.Size);
                    }

                    BufferOffset += Section.Size;
                }
            }

            for(auto Section : SHead)
                Bin.Write(Section);

            // seek to the front so the output is clean
            Bin.Seek(0);
            return Bin;
        }
    };
#endif

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

    struct SELFDecryptor
    {
        // input file
        FileSystem::BufferedFile* File;

        //internal data
        // headers
        SCE::Header SCEHead;
        SELF::Header SELFHead;
        ELF::Header ELFHead;
        Array<ELF::ProgramHeader> PHead;
        Array<ELF::SectionHeader> SHead;
        Array<SELF::ControlInfo> Controls;

        AppInfo Info;

        SELFDecryptor(FileSystem::BufferedFile* F)
            : File(F)
        {
            File->Seek(0);
        }

        SELF::ControlInfo ReadControlInfo()
        {
            SELF::ControlInfo Ctrl;

            Ctrl.Type = File->Read<Big<U32>>();
            Ctrl.Size = File->Read<Big<U32>>();
            Ctrl.Next = File->Read<Big<U64>>();

            if(Ctrl.Type == 1)
                Ctrl.ControlFlags = File->Read<decltype(SELF::ControlInfo::ControlFlags)>();
            else if(Ctrl.Type == 2 && Ctrl.Size == 48)
                Ctrl.ELFDigest40 = File->Read<decltype(SELF::ControlInfo::ELFDigest40)>();
            else if(Ctrl.Type == 2 && Ctrl.Size == 64)
                Ctrl.ELFDigest30 = File->Read<decltype(SELF::ControlInfo::ELFDigest30)>();
            else if(Ctrl.Type == 3)
                Ctrl.NPDRMInfo = File->Read<decltype(SELF::ControlInfo::NPDRMInfo)>();
            else
                VERROR("Invalid control type of %u", Ctrl.Type);

            return Ctrl;
        }

        bool ReadHeaders()
        {
            SCEHead = File->Read<SCE::Header>();

            if(SCEHead.Magic != "SCE\0"_U32)
            {
                VERROR("Invalid SCE magic");
                return false;
            }

            SELFHead = File->Read<SELF::Header>();
            Info = File->Read<AppInfo>();
            ELFHead = File->Read<ELF::Header>();

            if(ELFHead.Magic != "\177ELF"_U32)
            {
                VERROR("Invalid ELF magic");
                return false;
            }

            for(U32 I = 0; I < ELFHead.PHCount; I++)
                PHead.Append(File->Read<ELF::ProgramHeader>());

            File->Seek(SELFHead.ControlOffset);

            U32 C = 0;
            while(C < SELFHead.ControlLength)
            {
                auto Ctrl = ReadControlInfo();
                C += Ctrl.Size;

                Controls.Append(Ctrl);
            }

            File->Seek(SELFHead.SHeaderOffset);

            for(U32 I = 0; I < ELFHead.SHCount; I++)
                SHead.Append(File->Read<ELF::SectionHeader>());

            return true;
        }

        bool ReadMetadata(Byte* Key)
        {
            return false;
        }

        void DecryptData()
        {

        }

        ELF::Binary MakeELF()
        {   
            return {};
        }
#if UNSELFDEBUG
        void PrintData()
        {
            VINFO("SCE::Header Magic {}", SCEHead.Magic);
        }
#endif
    };

    namespace UNSELF
    {
        // public facing function to decrypt data
        Cthulhu::Option<ELF::Binary> DecryptSELF(Cthulhu::FileSystem::BufferedFile& File, Byte* Key)
        {
            // create our decryptor
            SELFDecryptor Decrypt(&File);

            // if we cant read in the headers log it and back out
            if(!Decrypt.ReadHeaders())
            {
                VERROR("Failed to read headers");
                return None<ELF::Binary>();
            }

#if UNSELFDEBUG
            Decrypt.PrintData();
#endif

            // if we cant read the metadata log it and back out
            if(!Decrypt.ReadMetadata(Key))
            {
                VERROR("Failed to read metadata");
                return None<ELF::Binary>();
            }

            // this function can never break
            Decrypt.DecryptData();

            // return the data in a nice format
            return Some(Decrypt.MakeELF());
        }
    }
}
