#pragma once

#include "Meta/Aliases.h"

namespace Volts
{
    using namespace Cthulhu;

    enum class KeyType : Cthulhu::U32
    {
        Level0 = 1,
        Level1 = 2,
        Level2 = 3,
        App = 4,
        DiskImage = 5,
        Loader = 6,
        Other = 7,
        NPDRM = 8,
    };

    namespace SELF
    {
        struct Key
        {
            U64 Version;
            U16 Revision;
            KeyType Type;
            U8 ERK[32];
            U8 RIV[16];
            U8 Public[40];
            U8 Private[21];
            U32 CurveType;
        };
    }

    SELF::Key GetSELFKey(KeyType Type, U16 Revision, U64 Version);
}