#include "thread.h"
#include "svl/endian.h"

#include <array>

#include <spdlog/spdlog.h>

using namespace svl;

namespace volts::ppu
{
    inline svl::u32 decode(svl::u32 op)
    {
        return (op >> 26 | op << 6) & 0x1ffff;
    }

    void thread::stub(form op)
    {
        spdlog::error("unimplemented ppu op {}", decode(op.raw));
    }

    void thread::ori(form op)
    {
        gpr[op.ra] = gpr[op.rs] | op.uimm16;
    }

    void thread::oris(form op)
    {

    }

    void thread::xori(form op)
    {
        gpr[op.ra] = gpr[op.rs] ^ op.uimm16;
    }

    void thread::xoris(form op)
    {

    }

    void thread::std(form op)
    {
        // TODO: write stuff
    }

    void thread::stdu(form op)
    {

    }

    void execute(svl::u32 op)
    {
        //ops[decode(op)](reinterpret_cast<form>(op));
    }
}