#include "PPUInterpreter.h"

#include <stdio.h>

namespace Volts::PS3
{
    void PPUInterpreter::Run(Cthulhu::Byte* Entry)
    {
        printf("%hhx\n", Entry[0]);
    }
}