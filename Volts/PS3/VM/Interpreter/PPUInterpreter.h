#pragma once

#include "PS3/VM/PPU.h"

namespace Volts::PS3
{
    struct PPUInterpreter : public PPU
    {
        virtual ~PPUInterpreter() override {}

        virtual void Run(Cthulhu::Binary& Bin) override;
        virtual Cthulhu::String Name() const override { return "Interpreter"; }
        virtual Cthulhu::String Description() const override { return "Execute PPU code without needing to recompile or perform extensive operations per instruction"; }
    };
}