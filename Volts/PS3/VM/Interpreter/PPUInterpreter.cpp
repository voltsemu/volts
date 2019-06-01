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

    void OP4SubMap(PPU& Thread, PPUInstruction Inst)
    {

    }

    // an array of function pointers
    // from page 181
    // http://math-atlas.sourceforge.net/devel/assembly/ppc_isa.pdf
    static void(*InstructionTable[])(PPU&, PPUInstruction) = {
        STUB, // 0: illegal reserved
        STUB, // 1: unused
        TDI, // 2: Trap Doubleword Immediate
        TWI, // 3: Trap Word Immediate
        OP4SubMap, // 4: possibly vector instructions
        STUB, // 5: unused
        STUB, // 6: unused
        MULLI, // 7: Multiply Low Immediate
        SUBFIC, // 8: Subtract From Immediate Carrying
        DOZI, // 9: Difference or Zero Immediate
        CMPLI, // 10: Compare Logical Immediate
        CMPI, // 11: Compare Immediate
    };

    void PPUInterpreter::Run(Cthulhu::Binary& Bin)
    {
        while(true)
        {

        }
    }
}