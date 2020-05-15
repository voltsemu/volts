#include "function.h"

#include <spdlog/spdlog.h>

namespace volts::ppu
{
    const char* func_name(svl::u32 fnid, const char* name)
    {
        if(!name)
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
        else if(strcmp(name, "sysPrxForUser") == 0)
        {
            switch(fnid)
            {
            case 0xBDB18F83: return "_sys_malloc";
            case 0xF7F7FB20: return "_sys_free";
            default: break;
            }
        }

        spdlog::warn("invalid fnid {}:{:x}", name, fnid);
        return "unkf";
    }

    const char* var_name(svl::u32 vnid, const char* name)
    {
        return "unkv";
    }
}