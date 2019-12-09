#pragma once

#include "svl/types.h"

namespace volts::vm
{
    using addr = svl::u32;

    svl::u8 read8(addr at);
    svl::u16 read16(addr at);
    svl::u32 read32(addr at);
    svl::u64 read64(addr at);
}