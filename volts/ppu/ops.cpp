#include "ops.h"

#include "thread.h"
#include "svl/endian.h"

#include <array>

#include <spdlog/spdlog.h>

namespace volts::ppu::ops
{
    inline svl::u32 decode(svl::u32 op)
    {
        return (op >> 26 | op << 6) & 0x1ffff;
    }

    void STUB(svl::u32 op, thread* t)
    {
        spdlog::error("unimplemented ppu op {}", decode(op));
    }

    void STD(svl::u32 op, thread* t)
    {

    }

    using opcode = void(*)(svl::u32, thread*);

    opcode ops[512] = {};

    void init_table()
    {
        ops[62] = STD;
    }

    void execute(svl::u32 op, thread* t)
    {
        spdlog::info(
            "raw = {:b} {} shifted = {} op = {}", 
            svl::endian::byte_swap(op), 
            svl::endian::byte_swap(op),
            (op >> 26 | op << (32 - 26)),
            (op >> 26 | op << (32 - 26)) & 0x1ffff
        );

        spdlog::info(
            "{:b}\n{:b}",
            (op >> 26 | op << 6),
            (op >> 26 | op << 6) & 0x1ffff
        );
    }
}