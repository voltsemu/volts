#pragma once

#include "Meta/Aliases.h"
#include <Core/Collections/CthulhuString.h>
#include <Core/Memory/Binary.h>
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

        template<typename T>
        void WriteCR(Cthulhu::U32 Field, T L, T R)
        {
            CR[Field * 4] = L < R;
            CR[Field * 4 + 1] = L > R;
            CR[Field * 4 + 2] = L == R;
            CR[Field * 4 + 3] = XER.SO;
        }

        Cthulhu::U64 LR = 0ULL; // link register

        Cthulhu::U64 CTR = 0ULL; // count register

        Cthulhu::U32 VRSave = 0U; // VRSAVE register

        Cthulhu::U32 InstrAddress = 0ULL; // current instruction address

        struct
        {
            bool SO;
            bool OV;
            bool CA;
            Cthulhu::U8 Count;
        } XER = {}; // fixed point exception register

        Cthulhu::U64 FPSCR = 0ULL; // floating point status and control register

        Cthulhu::U32 StackSize = 0U;
        Cthulhu::U32 StackAddress = 0U;

        virtual ~PPU() {}

        virtual void Run(Cthulhu::Binary& Bin) = 0;
        virtual Cthulhu::String Name() const = 0;
        virtual Cthulhu::String Description() const = 0;
    };

    // instruction format reference taken from
    // http://math-atlas.sourceforge.net/devel/assembly/ppc_isa.pdf
    union PPUInstruction
    {
        Cthulhu::U32 Raw;

        // the first 6 bits are always the opcode
        BitField<Cthulhu::U32, 0, 6> OPCode;

        BitField<Cthulhu::U32, 6, 24> LI;
        BitField<Cthulhu::U8, 6, 5> TO;
        BitField<Cthulhu::U32, 6, 5> RD;
        BitField<Cthulhu::U32, 6, 5> BO;
        BitField<Cthulhu::U32, 11, 5> BI;
        BitField<Cthulhu::U32, 16, 14> BD;
        BitField<Cthulhu::U32, 20, 7> LEV;
        BitField<Cthulhu::U32, 6, 5> RT;
        BitField<Cthulhu::U32, 11, 5> RA;
        BitField<Cthulhu::U32, 16, 15> D;
        BitField<Cthulhu::U32, 16, 15> SI;
        BitField<Cthulhu::U32, 16, 15> UI;
        BitField<Cthulhu::U32, 16, 14> DS;
        BitField<Cthulhu::U32, 30, 1> XO;
        BitField<Cthulhu::U32, 16, 16> SIMM16;

        BitField<bool, 30, 1> AA;
        BitField<bool, 31, 1> LK;
    };

    static_assert(sizeof(PPUInstruction) == sizeof(Cthulhu::U32));
}