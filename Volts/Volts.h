#include "Core/Collections/CthulhuString.h"
#include "Core/Macros.h"

#pragma once

namespace Volts
{
    struct Emulator
    {
        virtual ~Emulator() {}

        virtual void Init() = 0;
        virtual void Run() = 0;
        virtual void Pause() = 0;
        virtual Cthulhu::Byte* SaveState() = 0;
        virtual void Quit() = 0;
        virtual Cthulhu::String Name() = 0;
        virtual Cthulhu::String Description() = 0;
    };

    Cthulhu::Empty AddEmulator(Emulator* E);
#   define REGISTER_EMULATOR(Name) namespace { auto CAT(EMU, __LINE__) = AddEmulator(new Name()); }

    const char* Stub();
}