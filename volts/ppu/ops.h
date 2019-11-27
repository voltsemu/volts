#pragma once

#include "svl/types.h"

namespace volts::ppu
{
    struct thread;
}

namespace volts::ppu::ops
{
    void init_table();
    void execute(svl::u32 op, ppu::thread* t);
}