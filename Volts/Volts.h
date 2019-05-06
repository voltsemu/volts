#pragma once

#include <Core/Collections/Array.h>

namespace Volts
{
    struct Emulator
    {
        virtual ~Emulator() {}

        virtual void Run() = 0;

        virtual Cthulhu::String Name() = 0;
    };

    struct AddEmulator
    {
        AddEmulator(Emulator* Emu);
    };

#define CAT_INNER(A, B) A##B
#define CAT(A, B) CAT_INNER(A, B)

#define REGISTER_EMULATOR(T) namespace { static AddEmulator CAT(_, __LINE__) = {new T()}; }
}