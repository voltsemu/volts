#pragma once

#include "Meta/Aliases.h"

namespace Volts::PS3
{
    struct PPU
    {
        virtual ~PPU() {}

        virtual void Run(Cthulhu::Byte* Entry) = 0;

        Cthulhu::U64 GPR[32];
        Cthulhu::F64 FPR[32];
    };
}