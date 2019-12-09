#pragma once

#include "svl/types.h"
#include "svl/stream.h"

namespace volts::spu
{
    struct thread
    {
        svl::v128 gpr[128] = {};

        thread(svl::iostream& stream);

        // todo: float point status and control register
    };
}