
#include "Volts.h"

namespace Volts
{
    Cthulhu::Array<Emulator*> Emulators = {};

    AddEmulator::AddEmulator(Emulator* Emu)
    {
        Emulators.Append(Emu);
    }
}

int main(int argc, const char** argv)
{
    printf("Main\n");
    for(auto* E : Volts::Emulators)
    {
        printf("%s\n", E->Name().CStr());
    }
}
