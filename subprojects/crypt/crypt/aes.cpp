#include "smallcrypt/aes.h"

#include <svl/platform.h>
#include <svl/logger.h>

namespace smallcrypt
{
    void init()
    {
        uint32_t regs[4];
        CPUID(1, regs);
        if (regs[2] & 1 << 25)
        {
            svl::debug("AES-NI detected");
        }
        else
        {
            svl::debug("AES-NI unsupported");
        }
    }
}