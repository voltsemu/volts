#pragma once

#include "loader/elf.h"

namespace volts::ppu
{
    void load_prx(loader::elf::ppu_prx& mod);
}