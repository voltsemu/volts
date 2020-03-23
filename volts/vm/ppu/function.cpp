#include "function.h"

#include <fmt/fmt.h>

namespace volts::ppu
{
    const std::string& get_function_name(svl::u32 fnid, const std::string& mod_name)
    {
        if(mod_name.empty())
        {
            switch(fnid)
            {
            case 0x0D10FD3F: return "module_prologue";
            case 0x330F7005: return "module_epilogue";
            case 0x3AB9A95E: return "module_exit";
            case 0xBC9A0086: return "module_start";
            case 0xAB779874: return "module_stop";
            default: break;
            }
        }

        return "unkf";
    }

    const std::string& var_name(svl::u32 vnid, const std::string& mod_name)
    {
        return "unkv";
    }
}