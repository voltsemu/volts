#pragma once

#include <string>

#include <types.h>

namespace volts::ppu
{
    const char* function_name(svl::u32 fnid, const char* name = nullptr);
    const char* var_name(svl::u32 vnid, const char* name = nullptr);
}