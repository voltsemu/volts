#pragma once

#include "PPU.h"

namespace Volts::PS3
{
    struct PPUInterpreter : public PPU
    {
        virtual ~PPUInterpreter() override {}

        virtual void Run(Cthulhu::Byte* Entry) override; 
    };
}