#pragma once

#include <elf.h>
#include <map>

namespace volts::ppu
{
    void load_prx(elf::ppu_prx& mod);
    void load_exec(elf::ppu_exec& exec);
}