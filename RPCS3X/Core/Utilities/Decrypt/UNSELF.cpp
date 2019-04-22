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



/**
#include "Utilities/Convert.h"
#include <Core/Math/Math.h>

namespace RPCS3X
{
    bool IsSELF32(FS::BufferedFile& File)
    {
        if(!File.Valid())
        {
            return false;
        }

        File.Seek(sizeof(SCE::Header);

        auto SELFHead = File.Read<SELF::Header>();

        File.Seek(SELFHead.Offset);
        Array<U8> Class = File.ReadBytes(8);

        return (File.ReadBytes(8)[4] == 1);
    }

    bool IsDebug(FS::BufferedFile& File)
    {
        if(File.Size() < 18)
        {
            return false;
        }

        File.Seek(8);

        const auto Version = File.Read<Big<U16>>().Get();

        if (Version == 128 || Version == 192)
        {
            File.Seek(16);

            File.Seek((Version == 128) ? +File.Read<Big<U64>>().Get() : +File.Read<Little<U64>>().Get());
        
            auto E = FS::BufferedFile(new Array<U8>());

            const auto Len = Math::Min<U64>(File.CurrentDepth(), 2048);

            E.Write(File.ReadBytes(Len));

            S.Claim(E);

            return true;
        }

        return false;
    }

    struct SELFDecryptor
    {
        SELFDecryptor(FS::BufferedFile& File)
            : F(File)
        {}

        FS::BufferedFile& F;

        SCE::Header SCEHead;
        SELF::Header SELFHead;

        AppInfo Info;

        ELF::Header<U32> ELF32Head;
        Array<ELF::SectionHeader<U32>> SHeads32;
        Array<ELF::ProgramHeader<U32>> PHeads32;

        ELF::Header<U64> ELF64Head;
        Array<ELF::SectionHeader<U64>> SHeads64;
        Array<ELF::ProgramHeader<U64>> PHeads64;

        bool LoadHeaders(bool ELF32)
        {
            F.Seek(0);
            SCEHead = F.Read<SCE::Header>();

            if (!SCEHead.Valid())
            {
                return false;
            }

            SELFHead = F.Read<SELF::Header>();

            F.Seek(SELFHead.InfoOffset.Get());
            Info = F.Read<AppInfo>();

            F.Seek(SELFHead.ELFOffset.Get());

            if(ELF32)
            {
                ELF32Head = F.Read<ELF::Header<U32>>();

                if(ELF32Head.HeaderOffset.Get() == 0 && ELF32Head.ProgramEntries.Get() != 0)
                {
                    return false;
                }

                F.Seek(SELFHead.PHeaderOffset.Get());

                for(U32 I = 0; I < ELF32Head.ProgramEntries.Get(); I++)
                {
                    PHeads32.Append(F.Read<ELF::ProgramHeader<U32>>());
                }
            }
            else
            {
                ELF64Head = F.Read<ELF::Header<U64>>();

                if(ELF64Head.HeaderOffset.Get() == 0 && ELF64Head.ProgramEntries.Get() != 0)
                {
                    return false;
                }

                F.Seek(SELFHead.PHeaderOffset.Get());

                for (U32 I = 0; I < ELF64Head.ProgramEntries.Get(); I++)
                {
                    PHeads64.Append(F.Read<ELF::ProgramHeader<U64>>());
                }
            }

            F.Seek(SELFHead.VersionOffset.Get());
        }

        bool LoadMetaData(Array<U8>& Key)
        {
            return true;
        }

        bool DecryptBody()
        {
            return true;
        }

        Array<Byte> ToELF(bool ELF32)
        {
            return {};
        }
    };

    Result<SELFBinary, Error> DecryptSELF(FS::BufferedFile& File, Array<U8> Key)
    {
        if(!File.Valid())
        {
            return Fail<SELFBinary, Error>(Error::BadFile);
        }

        File.Seek(0);

        if(File.Size() >= 4 && File.Read<U32>() == "SCE\0"_U32 && !IsDebug(File))
        {
            bool ELF32 = IsSELF32(File);

            SELFDecryptor Decrypt(File);

            if(!Decrypt.LoadHeaders(ELF32))
            {
                return Fail<SELFBinary, Error>(Error::BadHeaders);
            }

            if(!Decrypt.LoadMetaData(Key))
            {
                return Fail<SELFBinary, Error>(Error::BadMetaData);
            }

            if(!Decrypt.DecryptBody())
            {
                return Fail<SELFBinary, Error>(Error::FailedDecrypt);
            }

            return Decrypt.ToELF(ELF32);
        }

        return Pass<SELFBinary, Error>({ File });
    }
}*/