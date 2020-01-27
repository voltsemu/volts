#pragma once

#include "svl/types.h"
#include "svl/file.h"

namespace volts::spu
{
    struct thread
    {
        svl::v128 gpr[128] = {};

        thread(svl::file stream);

        // todo: float point status and control register
    };
}