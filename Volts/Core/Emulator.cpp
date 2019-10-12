#include "Emulator.h"

namespace Volts
{
    Emulator* Emulator::Get()
    {
        static Emulator* Singleton = new Emulator();
        return Singleton;
    }
}