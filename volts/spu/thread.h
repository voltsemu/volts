#pragma once

#include "svl/types.h"

namespace volts::spu
{
    struct thread
    {
        svl::v128 gpr[128] = {};

        // todo: float point status and control register
    };
}