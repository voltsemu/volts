#include "Core/Collections/CthulhuString.h"
#include "Core/Macros.h"
#include "Core/Logger/Logger.h"

#pragma once

namespace Volts
{
    struct Emulator
    {
        virtual ~Emulator() {}

        virtual void Init() = 0;
        virtual void Run() = 0;
        virtual void Pause() = 0;
        virtual Cthulhu::Byte* SaveState() const = 0;
        virtual void Quit() = 0;
        virtual Cthulhu::String Name() const = 0;
        virtual Cthulhu::String Description() const = 0;
    };

    Cthulhu::Empty AddEmulator(Emulator* E);
#   define REGISTER_EMULATOR(Name) namespace { auto CAT(EMU, __LINE__) = AddEmulator(new Name()); }

    const char* Stub();

    struct NativeHandle
    {
        virtual ~NativeHandle() {}

        virtual void Init() = 0;
        virtual void* MakeWindow() = 0;
        void WriteLog(Level LogLevel, Cthulhu::String& Format, ...);
    };
}