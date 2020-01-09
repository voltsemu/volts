#pragma once

#include "svl/types.h"

namespace volts::vm
{
    using addr = svl::u32;

    svl::u8& read8(addr);
    svl::u16& read16(addr);
    svl::u32& read32(addr);
    svl::u64& read64(addr);

    void* real(addr of);

    void init();
}