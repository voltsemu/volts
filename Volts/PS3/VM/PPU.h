#pragma once

#include "Meta/Aliases.h"
#include "Core/Math/Bytes.h"
#include "Core/Endian.h"
#include <xmmintrin.h>

namespace Volts::PS3
{
    using Vec128 = __m128;

    struct PPU
    {
        Cthulhu::U64 GPR[32] = {}; // general purpous registers 
        Cthulhu::F64 FPR[32] = {}; // floating point registers
        Vec128 VPR[32] = {}; // vector registers 

        bool CR[32] = {}; // condition register, but unpacked

        Cthulhu::U64 LR = 0ULL; // link register

        Cthulhu::U64 CTR = 0ULL; // count register

        Cthulhu::U32 VRSave = 0U; // VRSAVE register

        Cthulhu::U32 InstrAddress = 0ULL; // current instruction address

        Cthulhu::U64 XER = 0ULL; // fixed point exception register

        Cthulhu::U64 FPSCR = 0ULL; // floating point status and control register

        Cthulhu::U32 StackSize = 0U;
        Cthulhu::U32 StackAddress = 0U;

        virtual void Init() = 0;
    };
}