#pragma once

#include "Core/SIMD/SIMD.h"

namespace Volts::PS3
{
    struct SPU
    {
        virtual ~SPU() {}
        virtual void Run() = 0;
        Vec128 GPR[128];
    };
}