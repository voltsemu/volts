#pragma once

#include "Meta/Aliases.h"
#include "Core/Math/Bytes.h"
#include "Core/Endian.h"
#include "Core/BitField.h"
#include "VM.h"

namespace Volts::PS3
{
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

    // instruction format reference taken from 
    // http://math-atlas.sourceforge.net/devel/assembly/ppc_isa.pdf
    union PPUInstruction
    {
        Cthulhu::U32 Raw;

        // the first 6 bits are always the opcode
        BitField<Cthulhu::U32, 0, 6> OPCode;

        BitField<Cthulhu::U32, 6, 24> LI;

        BitField<Cthulhu::U32, 6, 5> BO;
        BitField<Cthulhu::U32, 11, 5> BI;
        BitField<Cthulhu::U32, 16, 14> BD;

        BitField<Cthulhu::U32, 20, 7> LEV;

        BitField<Cthulhu::U32, 6, 5> RT;
        BitField<Cthulhu::U32, 11, 5> RA;
        BitField<Cthulhu::U32, 16, 15> D;

        BitField<bool, 30, 1> AA;
        BitField<bool, 31, 1> LK;
    };
}