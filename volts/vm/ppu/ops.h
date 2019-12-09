#include "thread.h"
#include "svl/endian.h"

#include "vm/vm.h"

#include <array>

#include <spdlog/spdlog.h>

using namespace svl;

namespace volts::ppu
{
    inline svl::u32 decode(svl::u32 op)
    {
        return (op >> 26 | op << 6) & 0x1ffff;
    }

    void stub(thread ppu, form op)
    {
        spdlog::error("unimplemented ppu op {}", decode(op.raw));
    }

    void ori(thread ppu, form op)
    {
        ppu.gpr[op.ra] = ppu.gpr[op.rs] | op.uimm16;
    }

    void oris(thread ppu, form op)
    {

    }

    void xori(thread ppu, form op)
    {
        ppu.gpr[op.ra] = ppu.gpr[op.rs] ^ op.uimm16;
    }

    void xoris(thread ppu, form op)
    {
        ppu.gpr[op.ra] = ppu.gpr[op.rs] ^ (op.uimm16 << 16);
    }

    void lbzx(thread ppu, form op)
    {
        
    }

    void lbzux(thread ppu, form op)
    {
        vm::addr addr = ppu.gpr[op.ra] + ppu.gpr[op.rb];
        ppu.gpr[op.rd] = vm::read8(addr);
        ppu.gpr[op.ra] = addr;
    }

    void lbz(thread ppu, form op)
    {
        vm::addr addr = op.ra ? ppu.gpr[op.ra] + op.simm16 : op.simm16;
        ppu.gpr[op.rd] = vm::read8(addr);
    }

    void lbzu(thread ppu, form op)
    {
        vm::addr addr = ppu.gpr[op.ra] + op.simm16;
        ppu.gpr[op.rd] = vm::read8(addr);
        ppu.gpr[op.ra] = addr;
    }

    void std(thread ppu, form op)
    {
        // TODO: write stuff
    }

    void stdu(thread ppu, form op)
    {

    }

    void execute(svl::u32 op)
    {
        //ops[decode(op)](reinterpret_cast<form>(op));
    }

    using func_t = void(*)(thread, form);
}