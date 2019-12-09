#include "thread.h"

#include "ops.h"

#include "svl/endian.h"

namespace volts::ppu
{
    thread::thread(svl::iostream& stream)
    {
        for(int i = 0; i < 10; i++)
        {
            // auto op = svl::read<svl::u32>(stream);
            
        }
    }
}