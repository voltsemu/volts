#pragma once

#include "loader/elf.h"

namespace volts::ppu
{
    void load_module(loader::elf::ppu_exec& mod);

    void init();
}