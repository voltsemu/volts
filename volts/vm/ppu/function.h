#pragma once

#include <string>

#include <types.h>

namespace volts::ppu
{
    const std::string& function_name(svl::u32 fnid, const std::string& mod_name = "");
    const std::string& var_name(svl::u32 vnid, const std::string& mod_name = "");
}