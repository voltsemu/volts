#include "SELF.h"

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

        bool LoadHeaders(bool ELF32)
        {
            return true;
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
}