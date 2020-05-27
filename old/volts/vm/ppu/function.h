#pragma once

#include <svl/types.h>

namespace volts::ppu
{
    const char* func_name(svl::u32 fnid, const char* name = nullptr);
    const char* var_name(svl::u32 vnid, const char* name = nullptr);
}