#pragma once

#include "Utilities/SFO.h"
#include "Utilities/Decrypt/SELF.h"

namespace RPCS3X
{
    using Str = Cthulhu::String;

    enum class Channel
    {
        Emu,
        GPU,
        CPU,
        IO,
        Audio,
        Utils,
    };

    struct Logger
    {
        void Debug(Channel C, Str Message);
        void Info(Channel C, Str Message);
        void Warn(Channel C, Str Message);
        void Error(Channel C, Str Message);
        void Fatal(Channel C, Str Message);
    };

    struct Emulator
    {

    };
}