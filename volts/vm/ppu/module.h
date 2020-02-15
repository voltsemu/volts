#pragma once

#include <elf.h>

namespace volts::ppu
{
    void load_prx(elf::ppu_prx& mod);
}