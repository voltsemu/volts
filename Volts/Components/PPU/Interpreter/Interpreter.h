#pragma once

#include "Components/PPU/PPU.h"

#include "Core/Emulator.h"

namespace Volts::PPU
{
    struct ErrorMessage
    {
        const char* Message;
    };

    auto Stub = [](PPUThread& Thread, PPUInstruction OP)
    {
        VFATAL("Unrecongised instruction {}", OP.Raw);
    };

    using Inst = void(*)(PPUThread&, PPUInstruction);

    Inst Instructions[] = {
        Stub, // 0: illegal reserved
        Stub, // 1: unused
        [](auto& Thread, auto OP) {}, // TDI 2: Trap Doubleword Immediate
        [](auto& Thread, auto OP) {}, // TWI 3: Trap Word Immediate
        [](auto& Thread, auto OP) {}, // 4: Vector extensions
        Stub, // 5: unused
        Stub, // 6: unused
        [](auto& Thread, auto OP) {}, // MULLI 7: Multiply Low Immediate
        [](auto& Thread, auto OP) {}, // SUBFIR 8: Subtract From Immediate Carrying
        [](auto& Thread, auto OP) {}, // DOZI 9: Difference or Zero Immediate
        [](auto& Thread, auto OP) {}, // CMPLI 10: Compare Logical Immediate
        [](auto& Thread, auto OP) {}, // CMPL 11: Compare Immediate
    };
}