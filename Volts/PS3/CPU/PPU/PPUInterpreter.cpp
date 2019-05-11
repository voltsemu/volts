#include "PPUInterpreter.h"

#include <stdio.h>

namespace Volts::PS3
{
    void PPUInterpreter::Run(Cthulhu::Byte* Entry)
    {
        printf("%lx\n", Entry[0]);
    }
}