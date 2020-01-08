#include "thread.h"
#include "svl/endian.h"

#include "vm/vm.h"

#include <array>

#include <spdlog/spdlog.h>

using namespace svl;

namespace volts::ppu
{
    svl::u32 decode(svl::u32 op)
    {
        return (op >> 26 | op << 6) & 0x1ffff;
    }

    void stub(thread& ppu, form op)
    {
        spdlog::error("unimplemented ppu op {}", decode(op.raw));
    }

    void tdi(thread& ppu, form op)
    {
        u64 a = ppu.gpr[op.ra];
        i64 s = (i64)op.simm16;
        
        if(
            ((a < s) && (op.bo & 0x10)) ||
            ((a > s) && (op.bo & 0x8)) ||
            ((a == s) && (op.bo & 0x4)) ||
            ((a < s) && (op.bo & 0x2)) ||
            ((a > s) && (op.bo & 0x1))
        )
        {
            spdlog::error("trap");
            // todo: pause
        }
    }

    void twi(thread& ppu, form op)
    {
        u32 a = ppu.gpr[op.ra];
        i64 s = (i64)op.simm16;

        if(
            ((a < s) && (op.bo & 0x10)) ||
            ((a > s) && (op.bo & 0x8)) ||
            ((a == s) && (op.bo & 0x4)) ||
            ((a < s) && (op.bo & 0x2)) ||
            ((a > s) && (op.bo & 0x1))
        )
        {
            spdlog::error("trap");
            // todo: pause
        }
    }

    void ori(thread& ppu, form op)
    {
        ppu.gpr[op.ra] = ppu.gpr[op.rs] | op.uimm16;
    }

    void oris(thread& ppu, form op)
    {
        ppu.gpr[op.ra] = ppu.gpr[op.rs] | ((u64)op.uimm16 << 16);
    }

    void xori(thread& ppu, form op)
    {
        ppu.gpr[op.ra] = ppu.gpr[op.rs] ^ op.uimm16;
    }

    void xoris(thread& ppu, form op)
    {
        ppu.gpr[op.ra] = ppu.gpr[op.rs] ^ ((u64)op.uimm16 << 16);
    }

    void lbz(thread& ppu, form op)
    {
        vm::addr addr = op.ra ? ppu.gpr[op.ra] + op.simm16 : op.simm16;
        ppu.gpr[op.rd] = vm::read8(addr);
    }

    void lbzu(thread& ppu, form op)
    {
        vm::addr addr = ppu.gpr[op.ra] + op.simm16;
        ppu.gpr[op.rd] = vm::read8(addr);
        ppu.gpr[op.ra] = addr;
    }

    void lbzx(thread& ppu, form op)
    {
        vm::addr addr = op.ra ? ppu.gpr[op.ra] + ppu.gpr[op.rb] : ppu.gpr[op.rb];
        ppu.gpr[op.rd] = vm::read8(addr);
    }

    void lbzux(thread& ppu, form op)
    {
        vm::addr addr = ppu.gpr[op.ra] + ppu.gpr[op.rb];
        ppu.gpr[op.rd] = vm::read8(addr);
        ppu.gpr[op.ra] = addr;
    }

    void std(thread& ppu, form op)
    {
        vm::addr addr = ppu.gpr[op.ra] + (op.simm16 & ~3);
        vm::read64(addr) = ppu.gpr[op.rs];
        ppu.gpr[op.ra] = addr;
    }

    void stdu(thread& ppu, form op)
    {
        vm::addr addr = ppu.gpr[op.ra] + (op.simm16 & ~3);
        vm::read64(addr) = ppu.gpr[op.rs];
        ppu.gpr[op.ra] = addr;
    }
    
    using func_t = void(*)(thread&, form);
    
    func_t ops[0x20000];

    struct instruction
    {
        u32 val;
        func_t func;
        u32 magn = 0;
    };

    // this is just stolen from rpcs3
    // at the point i find a better way of doing this ill use that instead
    void fill(u32 op, u32 num, u32 sh, std::initializer_list<instruction> instr)
    {
        if(sh < 11)
        {   
            for(auto& e : instr)
            {
                for(int i = 0; i < (e.magn + (11 - sh - num)); i++)
                {
                    for(int j = 0; j < 1 << sh; j++)
                    {
                        auto at = (((((i << (num - e.magn)) | e.val) << sh) | j) << 6) | op;
                        ops[at] = e.func;
                    }
                }
            }
        }
        else
        {
            for(auto& e : instr)
            {
                for(int i = 0; i < 1 << 11; i++)
                {
                    ops[i << 6 | e.val] = e.func;
                }
            }
        }
    }

    void init_table()
    {
        // fill the table with stub ops to stop random crashes
        std::fill(std::begin(ops), std::end(ops), stub);

        fill(0, 6, -1, {
            { 0x02, tdi },
            { 0x03, twi },

            { 0x18, ori },
            { 0x19, oris },
            { 0x1A, xori },
            { 0x1B, xoris },

            { 0x22, lbz },
            { 0x23, lbzu }
        });

        fill(0x3E, 2, 0, {
            { 0x0, std },
            { 0x1, stdu }
        });

        fill(0x1F, 10, 1, {
            { 0x57, lbzx },
            { 0x77, lbzux }
        });
    }
}