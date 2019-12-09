#include "thread.h"

#include "ops.h"

#include "svl/endian.h"

namespace volts::ppu
{
    std::array<func_t, 0x20000> ops;
    thread::thread(svl::iostream& stream)
    {
        for(int i = 0; i < 10; i++)
        {
            // auto op = svl::read<svl::u32>(stream);
            
        }
    }
}