#include "thread.h"

#include "ops.h"

#include "svl/endian.h"

namespace volts::ppu
{
    thread::thread(u32 entry)
    {
        init_table();
        cia = entry;

        for(int i = 0; i < 10; i++)
        {
            auto op = vm::read32(cia);
            auto d = decode(op);
            spdlog::info("{} {:x}", d, op);
            ops[d](*this, {op});
            cia += 4;
        }
    }
}