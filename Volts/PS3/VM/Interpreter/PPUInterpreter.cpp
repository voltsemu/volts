#include "PPUInterpreter.h"
#include "Core/Endian.h"
#include "Core/Logger/Logger.h"

namespace Volts::PS3
{
    using namespace Cthulhu;

    template<typename T>
    struct AddResult
    {
        AddResult(T L, T R)
            : Result(L + R)
            , Carry(Result < L)
            , Zero(Result == 0)
            , Sign(Result >> (sizeof(T) * 8 - 1) != 0)
        {}

        AddResult(T L, T R, bool B)
            : AddResult(L, R)
        {
            AddResult Res(Result, B);
            Result = Res.Result;
            Carry |= Res.Carry;
            Zero = Res.Zero;
            Sign = Res.Sign;
        }

        static AddResult<T> Flags(T L, T R)
        {
            return AddResult{L, R};
        }

        static AddResult<T> Flags(T L, T R, bool B)
        {
            return AddResult{L, R, B};
        }

        T Result;
        bool Carry;
        bool Zero;
        bool Sign;
    };

    void STUB(PPU& Thread, PPUInstruction OP)
    {
        LOGF_ERROR(PPU, "Invalid/Unrecognized OPCode: %u", OP.OPCode.Get());
    }

    void TDI(PPU& Thread, PPUInstruction OP)
    {

    }

    void TWI(PPU& Thread, PPUInstruction OP)
    {

    }

    void VecMap(PPU& Thread, PPUInstruction OP)
    {

    }

    void MULLI(PPU& Thread, PPUInstruction OP)
    {
        Thread.GPR[OP.RD] = (I64)Thread.GPR[OP.RA] * OP.SI;
    }

    void SUBFIC(PPU& Thread, PPUInstruction OP)
    {
        Thread.GPR[OP.RT] = Thread.GPR[OP.RA] + (I64)OP.SI + 1;
    }

    void DOZI(PPU& Thread, PPUInstruction OP)
    {

    }

    void CMPLI(PPU& Thread, PPUInstruction OP)
    {
        if(OP.L)
            Thread.WriteCR(OP.BF, (U32)Thread.GPR[OP.RA], (U32)OP.SI);
        else
            Thread.WriteCR(OP.BF, (I32)Thread.GPR[OP.RA], (I32)OP.SI);
    }

    void CMPL(PPU& Thread, PPUInstruction OP)
    {
        CMPLI(Thread, OP);
    }

    void ADDIC(PPU& Thread, PPUInstruction OP)
    {
        const auto R = AddResult<U64>::Flags(Thread.GPR[OP.RA], OP.SI);
        Thread.GPR[OP.RD] = R.Result;
        Thread.XER.CA = R.Carry;
    }

    void ADDIC2(PPU& Thread, PPUInstruction OP)
    {
        const auto R = AddResult<U64>::Flags(Thread.GPR[OP.RA], OP.SI);
        Thread.GPR[OP.RD] = R.Result;
        Thread.XER.CA = R.Carry;
        Thread.WriteCR(0, R.Result, 0ULL);
    }

    void ADDI(PPU& Thread, PPUInstruction OP)
    {
        Thread.GPR[OP.RT] = OP.RA ? Thread.GPR[OP.RA] + (I64)OP.SI : (I64)OP.SI;
    }

    void ADDIS(PPU& Thread, PPUInstruction OP)
    {
        Thread.GPR[OP.RT] = OP.RA ? OP.RA + (I64)Thread.GPR[OP.RA] + (OP.SI << 16) : OP.SI << 16;
    }

    void BC(PPU& Thread, PPUInstruction OP)
    {

    }

    void SC(PPU& Thread, PPUInstruction OP)
    {

    }

    void B(PPU& Thread, PPUInstruction OP)
    {
        // get the address to the next instruction
        const U32 Address = Thread.Cursor + 4;
        Thread.Cursor = OP.AA ? OP.DS : Thread.Cursor;
        if(OP.LK)
            Thread.LR = Address;
    }

    void CRMap(PPU& Thread, PPUInstruction OP)
    {

    }

    void RLWIMI(PPU& Thread, PPUInstruction OP)
    {

    }

    void RLWINM(PPU& Thread, PPUInstruction OP)
    {

    }

    void RLMI(PPU& Thread, PPUInstruction OP)
    {

    }

    void RLWNM(PPU& Thread, PPUInstruction OP)
    {

    }

    void ORI(PPU& Thread, PPUInstruction OP)
    {
        Thread.GPR[OP.RA] = Thread.GPR[OP.RS] | OP.SI;
    }

    void ORIS(PPU& Thread, PPUInstruction OP)
    {
        Thread.GPR[OP.RA] = Thread.GPR[OP.RS] | ((U64)OP.SI << 16);
    }

    void XORI(PPU& Thread, PPUInstruction OP)
    {
        Thread.GPR[OP.RA] = Thread.GPR[OP.RS] ^ OP.SI;
    }

    void XORIS(PPU& Thread, PPUInstruction OP)
    {
        Thread.GPR[OP.RA] = Thread.GPR[OP.RS] ^ ((U64)OP.SI << 16);
    }

    void ANDI(PPU& Thread, PPUInstruction OP)
    {
        Thread.GPR[OP.RA] = Thread.GPR[OP.RS] & OP.SI;
        Thread.WriteCR(0, Thread.GPR[OP.RA], 0);
    }

    void ANDIS(PPU& Thread, PPUInstruction OP)
    {
        Thread.GPR[OP.RA] = Thread.GPR[OP.RS] & ((U64)OP.SI << 16);
        Thread.WriteCR(0, Thread.GPR[OP.RA], 0);
    }

    void FXDMap(PPU& Thread, PPUInstruction OP)
    {

    }

    void FXEXMap(PPU& Thread, PPUInstruction OP)
    {

    }

    void LWZ(PPU& Thread, PPUInstruction OP)
    {

    }

    void LWZU(PPU& Thread, PPUInstruction OP)
    {

    }

    void LBZ(PPU& Thread, PPUInstruction OP)
    {

    }

    void LBZU(PPU& Thread, PPUInstruction OP)
    {

    }

    void STW(PPU& Thread, PPUInstruction OP)
    {

    }

    void STWU(PPU& Thread, PPUInstruction OP)
    {

    }

    void STB(PPU& Thread, PPUInstruction OP)
    {

    }

    void STBU(PPU& Thread, PPUInstruction OP)
    {

    }

    void LHZ(PPU& Thread, PPUInstruction OP)
    {

    }

    void LHZU(PPU& Thread, PPUInstruction OP)
    {

    }

    void LHA(PPU& Thread, PPUInstruction OP)
    {

    }

    void LHAU(PPU& Thread, PPUInstruction OP)
    {

    }

    void STH(PPU& Thread, PPUInstruction OP)
    {

    }

    void STHU(PPU& Thread, PPUInstruction OP)
    {

    }

    void LMW(PPU& Thread, PPUInstruction OP)
    {

    }

    void STMW(PPU& Thread, PPUInstruction OP)
    {

    }

    void LFS(PPU& Thread, PPUInstruction OP)
    {

    }

    void LFSU(PPU& Thread, PPUInstruction OP)
    {

    }

    void LFD(PPU& Thread, PPUInstruction OP)
    {

    }

    void LFDU(PPU& Thread, PPUInstruction OP)
    {

    }

    void STFS(PPU& Thread, PPUInstruction OP)
    {

    }

    void STFSU(PPU& Thread, PPUInstruction OP)
    {

    }

    void STFD(PPU& Thread, PPUInstruction OP)
    {

    }

    void STFDU(PPU& Thread, PPUInstruction OP)
    {

    }

    void OP56Map(PPU& Thread, PPUInstruction OP)
    {

    }

    void OP57Map(PPU& Thread, PPUInstruction OP)
    {

    }

    void DSFormMap1(PPU& Thread, PPUInstruction OP)
    {

    }

    void FPEMap(PPU& Thread, PPUInstruction OP)
    {

    }

    void OP60Map(PPU& Thread, PPUInstruction OP)
    {

    }

    void OP61Map(PPU& Thread, PPUInstruction OP)
    {

    }

    void DSFormMap2(PPU& Thread, PPUInstruction OP)
    {

    }

    void FPDMap(PPU& Thread, PPUInstruction OP)
    {

    }

    using MappedInstruction = void(*)(PPU&, PPUInstruction);

    // an array of function pointers
    // from page 181
    // http://math-atlas.sourceforge.net/devel/assembly/ppc_isa.pdf
    static MappedInstruction OPTable[] = {
        STUB, // 0: illegal reserved
        STUB, // 1: unused
        TDI, // 2: Trap Doubleword Immediate
        TWI, // 3: Trap Word Immediate
        VecMap, // 4: possibly vector OPructions
        STUB, // 5: unused
        STUB, // 6: unused
        MULLI, // 7: Multiply Low Immediate
        SUBFIC, // 8: Subtract From Immediate Carrying
        DOZI, // 9: Difference or Zero Immediate
        CMPLI, // 10: Compare Logical Immediate
        CMPL, // 11: Compare Immediate
        ADDIC, // 12: Add Immediate Carry
        ADDIC2, // 13: Add Immediate Carry and Record
        ADDI, // 14: Add Immediate
        ADDIS, // 15: Add Immediate Shifted
        BC, // 16: Branch Conditional
        SC, // 17: System Call
        B, // 18: Branch
        CRMap, // 19: Conditional Register Ops
        RLWIMI, // 20: Rotate Left Word Immediate. Then Mask Insert
        RLWINM, // 21: Rotate Left Word Immediate. Then AMD with Mask
        RLMI, // 22: Rotate Left Then Mask Insert
        RLWNM, // 23: Rotate Left Then AND With Mask
        ORI, // 24: OR Immediate
        ORIS, // 25: OR Immediate Shifted
        XORI, // 26: XOR Immediate
        XORIS, // 27: XOR Immediate Shifted
        ANDI, // 28: AND Immediate
        ANDIS, // 29: AND Immediate Shifted
        FXDMap, // 30: FX Dwd Rot
        FXEXMap, // 31: FX Extended Ops
        LWZ, // 32: Load Word And Zero
        LWZU, // 33: Load Word And Zero With Update
        LBZ, // 34: Load Byte And Zero
        LBZU, // 35: Load Byte And Zero With Update
        STW, // 36: Store Word
        STWU, // 37: Store Word With Update
        STB, // 38: Store Byte
        STBU, // 39: Store Byte With Update
        LHZ, // 40: Load Half And Zero
        LHZU, // 41: Load Half And Zero With Update
        LHA, // 42: Load Half Algebraic
        LHAU, // 43: Load Half Algebraic With Update
        STH, // 44: Store Half
        STHU, // 45: Store Half With Update
        LMW, // 46: Load Multiple Word
        STMW, // 47: Store Multiple Word
        LFS, // 48: Load Floating Point Single
        LFSU, // 49: Load Floating-Point Single with Update
        LFD, // 50: Load Floating-Point Double
        LFDU, // 51: Load Floating-Point Double with Update
        STFS, // 52: Store Floating-Point Single
        STFSU, // 53: Store Floating-Point Single with Update
        STFD, // 54: Store Floating-Point Double
        STFDU, // 55: Store Floating-Point Double with Update
        OP56Map, // 56: Extended Ops
        OP57Map, // 57: Extended Ops
        DSFormMap1, // 58: Extended Ops
        FPEMap, // 59: Extended Float Ops
        OP60Map, // 60: Extended Ops
        OP61Map, // 61: Extended Ops
        DSFormMap2, // 62: Extended Ops
        FPDMap, // 63: Extended Double Ops
    };

    void PPUInterpreter::Run(Cthulhu::Binary& Bin)
    {
        Bin.Seek(0x18);
        U64 Entry = Bin.Read<Big<U64>>();
        printf("Entry %llu\n", Entry);
        Bin.Seek(Entry);
        for(U32 _ = 0; _ < 10; _++)
        {
            auto Inst = Bin.Read<PPUInstruction>();
            OPTable[Inst.OPCode](*this, Inst);
        }
    }
}