#pragma once

#include "Meta/Aliases.h"

#include <intrin.h>

namespace Volts::PS3
{
    using Vec128 = __m128;

    // the current state of the ppu thread
    // this is different from the general ppu state
    // as this represents emulator state rather than console state
    enum class PPUThreadState : Cthulhu::U8
    {
        None, // thread hasnt been started yet
        Paused, // thread has been paused by emulator
        Locked, // thread has been locked while state is being transfered
        Running, // thread is running normally
    };  

    // all variables that represent a PPU threads state
    struct PPUState
    {
        Cthulhu::U32 GPR[32]; // general purpose registers
        Cthulhu::F32 FPR[32]; // floating point registers
        Vec128 VR[32]; // vector registers

        bool CR[32]; // condition registers
    };

    struct PPU : public PPUState
    {
        
    };

    struct SPU
    {
        Vec128 VR[128];
    };

    struct RSX
    {

    };

    struct State
    {

    };

    struct VM
    {
        Cthulhu::Byte* Memory;
    };
}