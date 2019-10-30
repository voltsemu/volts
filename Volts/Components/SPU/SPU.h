#pragma once

#include "Core/Types.h"

namespace Volts::SPU
{
    using namespace Cthulhu;
    using namespace Types;

    struct SPU
    {
        V128 GPR[128] = {};

        struct
        {
            U32 Fields[4];
        } FPSCR;
    };
}