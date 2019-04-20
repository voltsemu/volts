#pragma once

#include <Core/Collections/Array.h>
#include <Core/Collections/CthulhuString.h>

namespace RPCS3X
{
    using Str = Cthulhu::String;
    using LogChannels = Cthulhu::Array<Str>;
    struct Logger
    {
        LogChannels Channels;
        Logger& AddChannel(Str& Channel);
    };
}