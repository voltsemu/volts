#include "thread.h"

#include "ops.h"

#include "svl/endian.h"

namespace volts::ppu
{
    thread::thread(u32 entry)
    {
        if(static bool table_flag = true; table_flag)
        {
            init_table();
            table_flag = false;
        }

        cia = entry;
        spdlog::info("entry point: {}", cia);

        for(int i = 0; i < 10; i++)
        {
            auto op = vm::read32(cia);
            auto d = decode(op);
            ops[d](*this, {op});
            cia += 4;
        }
    }
}