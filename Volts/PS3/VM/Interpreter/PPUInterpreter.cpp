#include "PPUInterpreter.h"
#include "Core/Logger/Logger.h"

namespace Volts::PS3
{
    void STUB(PPU& Thread, PPUInstruction Inst)
    {
        LOGF_ERROR(PPU, "Invalid/Unrecognized OPCode: %u", Inst.OPCode.Get());
    }

    void TDI(PPU& Thread, PPUInstruction Inst)
    {

    }

    void TWI(PPU& Thread, PPUInstruction Inst)
    {

    }

    void VecMap(PPU& Thread, PPUInstruction Inst)
    {

    }

    void MULLI(PPU& Thread, PPUInstruction Inst)
    {

    }

    void SUBFIC(PPU& Thread, PPUInstruction Inst)
    {

    }

    void DOZI(PPU& Thread, PPUInstruction Inst)
    {

    }

    void CMPLI(PPU& Thread, PPUInstruction Inst)
    {

    }

    void CMPI(PPU& Thread, PPUInstruction Inst)
    {

    }

    void ADDIC(PPU& Thread, PPUInstruction Inst)
    {

    }

    void ADDIC2(PPU& Thread, PPUInstruction Inst)
    {

    }

    void ADDI(PPU& Thread, PPUInstruction Inst)
    {

    }

    void ADDIS(PPU& Thread, PPUInstruction Inst)
    {

    }

    void BC(PPU& Thread, PPUInstruction Inst)
    {

    }

    void SC(PPU& Thread, PPUInstruction Inst)
    {

    }

    void B(PPU& Thread, PPUInstruction Inst)
    {

    }

    void CRMap(PPU& Thread, PPUInstruction Inst)
    {

    }

    void RLWIMI(PPU& Thread, PPUInstruction Inst)
    {

    }

    void RLWINM(PPU& Thread, PPUInstruction Inst)
    {

    }

    void RLMI(PPU& Thread, PPUInstruction Inst)
    {

    }

    void RLWNM(PPU& Thread, PPUInstruction Inst)
    {

    }

    void ORI(PPU& Thread, PPUInstruction Inst)
    {

    }

    void ORIS(PPU& Thread, PPUInstruction Inst)
    {

    }

    void XORI(PPU& Thread, PPUInstruction Inst)
    {

    }

    void XORIS(PPU& Thread, PPUInstruction Inst)
    {

    }

    void ANDI(PPU& Thread, PPUInstruction Inst)
    {

    }

    void ANDIS(PPU& Thread, PPUInstruction Inst)
    {

    }

    void FXDMap(PPU& Thread, PPUInstruction Inst)
    {

    }

    void FXEXMap(PPU& Thread, PPUInstruction Inst)
    {

    }

    void LWZ(PPU& Thread, PPUInstruction Inst)
    {

    }

    void LWZU(PPU& Thread, PPUInstruction Inst)
    {

    }

    void LBZ(PPU& Thread, PPUInstruction Inst)
    {

    }

    void LBZU(PPU& Thread, PPUInstruction Inst)
    {

    }

    void STW(PPU& Thread, PPUInstruction Inst)
    {

    }

    void STWU(PPU& Thread, PPUInstruction Inst)
    {

    }

    void STB(PPU& Thread, PPUInstruction Inst)
    {

    }

    void STBU(PPU& Thread, PPUInstruction Inst)
    {

    }

    void LHZ(PPU& Thread, PPUInstruction Inst)
    {

    }

    void LHZU(PPU& Thread, PPUInstruction Inst)
    {

    }

    void LHA(PPU& Thread, PPUInstruction Inst)
    {

    }

    void LHAU(PPU& Thread, PPUInstruction Inst)
    {

    }

    void STH(PPU& Thread, PPUInstruction Inst)
    {

    }

    void STHU(PPU& Thread, PPUInstruction Inst)
    {

    }

    void LMW(PPU& Thread, PPUInstruction Inst)
    {

    }

    void STMW(PPU& Thread, PPUInstruction Inst)
    {

    }

    void LFS(PPU& Thread, PPUInstruction Inst)
    {

    }

    void LFSU(PPU& Thread, PPUInstruction Inst)
    {

    }

    void LFD(PPU& Thread, PPUInstruction Inst)
    {

    }

    void LFDU(PPU& Thread, PPUInstruction Inst)
    {

    }

    void STFS(PPU& Thread, PPUInstruction Inst)
    {

    }

    void STFSU(PPU& Thread, PPUInstruction Inst)
    {

    }

    void STFD(PPU& Thread, PPUInstruction Inst)
    {

    }

    void STFDU(PPU& Thread, PPUInstruction Inst)
    {

    }

    void OP56Map(PPU& Thread, PPUInstruction Inst)
    {

    }

    void OP57Map(PPU& Thread, PPUInstruction Inst)
    {

    }

    void DSFormMap1(PPU& Thread, PPUInstruction Inst)
    {

    }

    void FPEMap(PPU& Thread, PPUInstruction Inst)
    {

    }

    void OP60Map(PPU& Thread, PPUInstruction Inst)
    {

    }

    void OP61Map(PPU& Thread, PPUInstruction Inst)
    {

    }

    void DSFormMap2(PPU& Thread, PPUInstruction Inst)
    {

    }

    void FPDMap(PPU& Thread, PPUInstruction Inst)
    {
        
    }

    using MappedInstruction = void(*)(PPU&, PPUInstruction);
    
    // an array of function pointers
    // from page 181
    // http://math-atlas.sourceforge.net/devel/assembly/ppc_isa.pdf
    static MappedInstruction InstructionTable[] = {
        STUB, // 0: illegal reserved
        STUB, // 1: unused
        TDI, // 2: Trap Doubleword Immediate
        TWI, // 3: Trap Word Immediate
        VecMap, // 4: possibly vector instructions
        STUB, // 5: unused
        STUB, // 6: unused
        MULLI, // 7: Multiply Low Immediate
        SUBFIC, // 8: Subtract From Immediate Carrying
        DOZI, // 9: Difference or Zero Immediate
        CMPLI, // 10: Compare Logical Immediate
        CMPI, // 11: Compare Immediate
        ADDIC, // 12: Add Immediate Carry
        ADDICR, // 13: Add Immediate Carry and Record
        ADDI, // 14: Add Immediate
        ADDIS, // 15: Add Immediate Shifted
    };

    void PPUInterpreter::Run(Cthulhu::Binary& Bin)
    {
        while(true)
        {

        }
    }
}