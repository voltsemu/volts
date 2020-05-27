#include "thread.h"
#include <svl/endian.h>
#include <svl/bitrange.h>

#include "vm.h"

#include <array>

#include <spdlog/spdlog.h>

namespace volts::ppu
{
    union form
    {
        svl::u32 raw;

        svl::bitrange<svl::u32, 30, 1> aa;

        svl::bitrange<svl::u32, 6, 5> rs;
        svl::bitrange<svl::u32, 11, 5> ra;
        svl::bitrange<svl::u32, 16, 5> rb;
        svl::bitrange<svl::u32, 6, 5> rd;

        svl::bitrange<svl::u32, 11, 5> va;
        svl::bitrange<svl::u32, 16, 5> vb;
        svl::bitrange<svl::u32, 21, 5> vc;
        svl::bitrange<svl::u32, 6, 5> vd;

        svl::bitrange<svl::u32, 6, 5> bo;

        svl::bitrange<svl::u32, 16, 14> ds;

        svl::bitrange<svl::u32, 6, 5> frs;
        svl::bitrange<svl::u32, 6, 5> frd;

        svl::bitrange<svl::u32, 31, 1> lk;

        svl::bitrange<svl::i32, 16, 16> simm16;
        svl::bitrange<svl::u32, 16, 16> uimm16;

        svl::bitrange<svl::i32, 16, 14> bt14;
        svl::bitrange<svl::i32, 6, 24> bt24;
    };

    static_assert(sizeof(form) == sizeof(svl::u32));

    using namespace svl;

    svl::u32 decode(svl::u32 op)
    {
        return (op >> 26 | op << (32 - 26)) & 0x1FFFF; 
    }

    void tdi(thread& ppu, form op)
    {
        u64 a = ppu.gpr[op.ra];
        u64 s = (u64)op.simm16;
        
        if(
            ((a < s) && (op.bo & 0x10)) ||
            ((a > s) && (op.bo & 0x8)) ||
            ((a == s) && (op.bo & 0x4)) ||
            ((a < s) && (op.bo & 0x2)) ||
            ((a > s) && (op.bo & 0x1))
        )
        {
            spdlog::critical("trap dword");
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
            spdlog::critical("trap word");
            // todo: pause
        }
    }

    void stw(thread& ppu, form op)
    {
        u64 addr = op.ra ? ppu.gpr[op.ra] + op.simm16 : (i32)op.simm16;
        u32 val = ppu.gpr[op.rs];
        vm::write<u32>(addr, val);
    }

    void stwu(thread& ppu, form op)
    {
        u64 addr = ppu.gpr[op.ra] + op.simm16;
        vm::write<u32>(addr, ppu.gpr[op.rs]);
        ppu.gpr[op.ra] = addr;
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
        vm::addr addr = op.ra ? ppu.gpr[op.ra] + op.simm16 : (i32)op.simm16;
        ppu.gpr[op.rd] = vm::read<u8>(addr);
    }

    void lbzu(thread& ppu, form op)
    {
        vm::addr addr = ppu.gpr[op.ra] + op.simm16;
        ppu.gpr[op.rd] = vm::read<u8>(addr);
        ppu.gpr[op.ra] = addr;
    }

    void lbzx(thread& ppu, form op)
    {
        vm::addr addr = op.ra ? ppu.gpr[op.ra] + ppu.gpr[op.rb] : ppu.gpr[op.rb];
        ppu.gpr[op.rd] = vm::read<u8>(addr);
    }

    void lbzux(thread& ppu, form op)
    {
        vm::addr addr = ppu.gpr[op.ra] + ppu.gpr[op.rb];
        ppu.gpr[op.rd] = vm::read<u8>(addr);
        ppu.gpr[op.ra] = addr;
    }

    void _std(thread& ppu, form op)
    {
        vm::addr addr = ppu.gpr[op.ra] + (op.simm16 & ~3);
        vm::write<u64>(addr, ppu.gpr[op.rs]);
        ppu.gpr[op.ra] = addr;
    }

    void stdu(thread& ppu, form op)
    {
        vm::addr addr = ppu.gpr[op.ra] + (op.simm16 & ~3);
        vm::write<u64>(addr, ppu.gpr[op.rs]);
        ppu.gpr[op.ra] = addr;
    }
    
    void lfs(thread& ppu, form op)
    {
        // TODO: is this right
        vm::addr addr = op.ra ? ppu.gpr[op.ra] + op.simm16 : (i32)op.simm16;
        ppu.fpr[op.frd] = vm::ref<f32>(addr);
    }

    void stfs(thread& ppu, form op)
    {
        vm::addr addr = op.ra ? ppu.gpr[op.ra] + op.simm16 : (i32)op.simm16;
        vm::write<f32>(addr, ppu.fpr[op.frs]);
    }

    void lhzu(thread& ppu, form op)
    {
        vm::addr addr = ppu.gpr[op.ra] + op.simm16;
        ppu.gpr[op.rd] = vm::read<u16>(addr);
        ppu.gpr[op.ra] = addr;
    }

    void lwzu(thread& ppu, form op)
    {
        vm::addr addr = ppu.gpr[op.ra] + op.simm16;
        ppu.gpr[op.rd] = vm::read<u32>(addr);
        ppu.gpr[op.ra] = addr;
    }

    void lhau(thread& ppu, form op)
    {
        vm::addr addr = ppu.gpr[op.ra] + op.simm16;
        ppu.gpr[op.rd] = vm::read<u16>(addr);
        ppu.gpr[op.ra] = addr;
    }

    void andi(thread& ppu, form op)
    {
        ppu.gpr[op.ra] = ppu.gpr[op.rs] & op.uimm16;
        // TODO: set cr

    }

    void b(thread& ppu, form op)
    {
        vm::addr link = ppu.cia + 4;
        ppu.cia = (op.aa ? 0 : ppu.cia) + op.bt24;

        if(op.lk) ppu.link = link;
    }

    void vmhaddshs(thread& ppu, form op)
    {
        auto a = ppu.vr[op.va].ints;
        auto b = ppu.vr[op.vb].ints;
        auto c = ppu.vr[op.vc].ints;

        auto m = _mm_or_si128(_mm_srli_epi16(_mm_mullo_epi16(a, b), 15), _mm_slli_epi16(_mm_mulhi_epi16(a, b), 1));
        auto s = _mm_cmpeq_epi16(m, _mm_set1_epi16(-0x8000));

        ppu.vr[op.vd].ints = _mm_adds_epi16(_mm_adds_epi16(_mm_xor_si128(m, s), c), _mm_srli_epi16(s, 15));
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
            for(const auto& e : instr)
            {
                for(u32 i = 0; i < (e.magn + (11 - sh - num)); i++)
                {
                    for(u32 j = 0; j < 1u << sh; j++)
                    {
                        ops[((((((i << (num - e.magn)) | e.val) << sh) | j) << 6) | op)] = e.func;
                    }
                }
            }
        }
        else
        {
            for(const auto& e : instr)
            {
                for(u32 i = 0; i < 1u << 11; i++)
                {
                    ops[i << 6 | e.val] = e.func;
                }
            }
        }
    }

    void init_table()
    {
        // fill the table with stub ops to detect invalid opcodes
        std::fill(std::begin(ops), std::end(ops), [](thread& t, form op) {
            spdlog::critical("invalid op {:x} at {:x}", op.raw, t.cia);
        });

        fill(0, 6, -1, {
            { 0x02, tdi },
            { 0x03, twi },

            { 0x12, b },

            { 0x18, ori },
            { 0x19, oris },
            { 0x1A, xori },
            { 0x1B, xoris },
            { 0x1C, andi },

            { 0x21, lwzu },
            { 0x22, lbz },
            { 0x23, lbzu },

            { 0x24, stw },
            { 0x25, stwu },

            { 0x29, lhzu },

            { 0x2B, lhau },

            { 0x30, lfs },

            { 0x34, stfs }
        });

        fill(0x3E, 2, 0, {
            { 0x0, _std },
            { 0x1, stdu }
        });

        fill(0x1F, 10, 1, {
            { 0x57, lbzx },
            { 0x77, lbzux }
        });

        fill(0x04, 11, 0, {
            { 0x20, vmhaddshs, 5 }
        });
    }
}