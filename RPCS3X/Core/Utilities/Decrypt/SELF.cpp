#include "SELF.h"

namespace RPCS3X
{
    SELFBinary::SELFBinary(FS::BufferedFile F)
        : File(F)
    {}

    Error SELFBinary::Load()
    {
        File.Seek(0);
        const auto SCEHead = File.Read<SCE::Header>();

        if(!SCEHead.Valid())
        {
            return Error::BadSCEHeader;
        }

        const auto SELFHead = File.Read<SELF::Header>();

        File.Seek(SELFHead.InfoOffset.Get());
        Info = File.Read<AppInfo>();

        File.Seek(SELFHead.ELFOffset.Get());
    }
}