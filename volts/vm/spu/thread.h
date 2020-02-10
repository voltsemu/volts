#pragma once

#include <types.h>
#include <file.h>

namespace volts::spu
{
    struct thread
    {
        svl::v128 gpr[128] = {};

        thread(svl::file stream);

        // todo: float point status and control register
    };
}