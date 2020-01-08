#pragma once

#include "svl/types.h"
#include "svl/bitfield.h"
#include "svl/stream.h"

namespace volts::ppu
{
    union control
    {
        svl::u8 bytes[32];
        svl::u32 fields[8];
    };

    static_assert(sizeof(control) == 32);

    union form
    {
        svl::u32 raw;

        svl::bit_field<svl::u32, 6, 11> rs;
        svl::bit_field<svl::u32, 11, 16> ra;
        svl::bit_field<svl::u32, 16, 20> rb;
        svl::bit_field<svl::u32, 6, 10> rd;
        svl::bit_field<svl::u32, 16, 30> ds;

        svl::bit_field<svl::u32, 6, 10> bo;

        svl::bit_field<svl::i32, 16, 31> simm16;
        svl::bit_field<svl::u32, 16, 31> uimm16;
    };

    static_assert(sizeof(form) == sizeof(svl::u32));

    struct thread
    {
        thread(svl::iostream& stream);
        
        svl::u64 gpr[32] = {};
        svl::f64 fpr[32] = {};

        svl::u64 link = 0;
        svl::u64 count = 0;
        svl::u64 xer = 0;

        control cr = {};
        // todo: fixed point exception

        svl::v128 vr[32] = {};

        // todo: vector status
        // vr save register
    };
}