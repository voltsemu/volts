#pragma once

#include "Utilities/SFO.h"
#include "Utilities/Decrypt/SELF.h"

namespace RPCS3X
{
    using Str = Cthulhu::String;

    struct Logger
    {
        static Array<String> Channels;
        void Debug(Str Channel, Str Message);
        void Info(Str Channel, Str Message);
        void Warn(Str Channel, Str Message);
        void Error(Str Channel, Str Message);
        void Fatal(Str Channel, Str Message);
    };

    struct Emulator
    {

    };
}