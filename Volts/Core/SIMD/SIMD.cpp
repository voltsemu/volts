#include "SIMD.h"
#include "Core/Macros.h"

namespace Volts
{
    using namespace Cthulhu;

    Instruction ISA;
    
    VSCRIPT({
        CPUFlag Registers[4] = {};

        CPUID(1, Registers);

        if(Registers[3] & MMX_FLAG)
            ISA |= Instruction::MMX;
        
        if(Registers[3] & SSE_FLAG)
            ISA |= Instruction::SSE;

        if(Registers[3] & SSE2_FLAG)
            ISA |= Instruction::SSE2;

        if(Registers[3] & AES_FLAG)
            ISA |= Instruction::AES;

        if(Registers[3] & TSC_FLAG)
            ISA |= Instruction::TSC;

        if(Registers[2] & SSE3_FLAG)
            ISA |= Instruction::SSE3;

        if(Registers[2] & SSSE3_FLAG)
            ISA |= Instruction::SSSE3;

        if(Registers[2] & SSE41_FLAG)
            ISA |= Instruction::SSE41;

        if(Registers[2] & SSE42_FLAG)
            ISA |= Instruction::SSE42;

        if(Registers[2] & AVX_FLAG)
            ISA |= Instruction::AVX;

        CPUID(7, Registers);

        if(Registers[1] & AVX2_FLAG)
            ISA |= Instruction::AVX2;

        if(Registers[1] & TSX_FLAG)
            ISA |= Instruction::TSX;

        if(Registers[1] & HLE_FLAG)
            ISA |= Instruction::HLE;

        if(Registers[1] & SHA_FLAG)
            ISA |= Instruction::SHA;

        if(Registers[1] & RTM_FLAG)
            ISA |= Instruction::RTM;

        if(Registers[1] & AVX512_FLAG)
            ISA |= Instruction::AVX512;

        if(Registers[2] & VAES_FLAG)
            ISA |= Instruction::VAES;

        CPUID(0x80000001, Registers);

        if(Registers[2] & SSE4A_FLAG)
            ISA |= Instruction::SSE4A;
    })

    Instruction SupportedInstructions()
    {
        return ISA;
    }
}
