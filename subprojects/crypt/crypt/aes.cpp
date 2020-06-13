#include "crypt/aes.h"

#include <svl/platform.h>
#include <svl/logger.h>

namespace
{
    // do we have aes-ni?
    bool aesni = false;

    // if we use static rom tables then we can stub out gen_tables()
    // the compiler should be smart enough to see the function is empty
    // and inline it away
#if ROM_TABLES
    void gen_tables() {}
#else
    void gen_tables()
    {

    }
#endif

}

namespace crypt
{
    void init()
    {
        uint32_t regs[4];
        CPUID(1, regs);
        aesni = regs[2] & (1 << 25);
        gen_tables();

        if (regs[2] & 1 << 25)
        {
            svl::debug("AES-NI detected");
        }
        else
        {
            svl::debug("AES-NI unsupported");
        }
    }

    Context Context::enc(const uint8_t* key)
    {

    }

    Context Context::dec(const uint8_t* key)
    {

    }
}