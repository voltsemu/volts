#pragma once

#include "PS3/VM/PPU.h"

namespace Volts::PS3
{
    struct PPUInterpreter : public PPU
    {
        virtual void Init() override;
    };
}