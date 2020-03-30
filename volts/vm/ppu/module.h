#pragma once

#include <elf.h>
#include <map>

namespace volts::ppu
{
    void load_prx(elf::ppu_prx& mod);
    void load_exec(elf::ppu_exec& exec);

    struct variable_info
    {
        const char* name;
        const char* type;
        std::vector<const char*> args;
    };

    struct function_info
    {
        const char* name;
        const char* type;
    };

    template<typename T>
    struct linkage_info
    {
        T data;
    };

    struct linkage_module
    {
        std::map<svl::u32, linkage_info<function_info>> functions;
        std::map<svl::u32, linkage_info<variable_info>> variables;
    };
}