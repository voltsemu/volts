#pragma once

#include "Volts/Platform/Interface/EmulatorWindow.h"

#include <Core/Collections/Array.h>
#include <Core/Collections/CthulhuString.h>

namespace Volts
{
    using Str = Cthulhu::String;

    namespace Log
    {
        extern Level LogLevel;

        void Info(const Str& Channel, const Str& Message);
        void Trace(const Str& Channel, const Str& Message);
        void Debug(const Str& Channel, const Str& Message);
        void Warning(const Str& Channel, const Str& Message);
        void Error(const Str& Channel, const Str& Message);
        void Fatal(const Str& Channel, const Str& Message);
    
        void Write(const Str& Channel, Level S, const Str& Message);
    };
}