#pragma once

#include <Meta/Aliases.h>

namespace Volts::Cell
{
    using namespace Cthulhu;

    struct PPUThread
    {
        // general registers
        U64 GPR[32] = {};

        // floating point registers
        F64 FPR[32] = {};

        // vector registers
        // TODO: implement v128 type
        // 128 bit unsigned integer
        // V128 VR[32] = {};

        // condition registers
        struct 
        {
            U8 Bits[32] = {};
            U32 Fields[8] = {};
        } CR;
    };
}