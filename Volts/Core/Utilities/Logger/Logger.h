#pragma once

#include <Core/Collections/Array.h>
#include <Core/Collections/CthulhuString.h>

namespace Volts
{
    using Str = Cthulhu::String;
    using LogChannels = Cthulhu::Array<Str>;
    struct Logger
    {
        LogChannels Channels;
        Logger& AddChannel(Str& Channel);
    };
}