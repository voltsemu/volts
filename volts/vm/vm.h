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

    // ps3 page size
    constexpr svl::i32 page_size = 0x1000;

    inline svl::u32 page_align(svl::u32 val) { return (val + page_size - 1) & -page_size; }
    inline svl::u32 round_down(svl::u32 val, svl::i32 to) { return val & -to; }
}