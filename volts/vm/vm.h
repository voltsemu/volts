#pragma once

#include "svl/types.h"

namespace volts::vm
{
    using addr = svl::u32;

    svl::u8 read8(addr at);
    svl::u16 read16(addr at);
    svl::u32 read32(addr at);
    svl::u64 read64(addr at);

    void write8(addr at, svl::u8 val);
    void write16(addr at, svl::u16 val);
    void write32(addr at, svl::u32 val);
    void write64(addr at, svl::u64 val);
}