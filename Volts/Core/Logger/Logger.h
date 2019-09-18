#pragma once

#include <stdio.h>
#include <Core/Collections/CthulhuString.h>
#include "Volts/Config.h"

#include <fmt/format.h>

#if OS_WINDOWS
// this header redefines OS_WINDOWS so we do this to stop any errors happening
#   pragma push_macro("OS_WINDOWS")
#   undef OS_WINDOWS
#   include <atlstr.h>
#   pragma pop_macro("OS_WINDOWS")
#endif

namespace Volts
{
    // how bad is the thing we logged?
    enum class Level
    {
        // info the end user might care about, such as the path of the game executable, etc
        Info = 0,
        // something bad has happened.
        // but not bad enough to warrant crashing or exiting.
        Warning = 1,
        // something very bad has happened.
        // may have damaged emulator integrity
        // some stuff may stop functioning
        Error = 2,
        // something so bad has happened the emulator has exited non gracefully
        // possibly internal emulator error or some very bad data was passed in
        Fatal = 3,
    };

    extern Level LogLevel;

    void BeginLogs();
    void ClearLogs();

    // print the associated prefix for a level
    void Print(Level L, std::string Msg);
}

#define INFO(...) { Volts::Print(Volts::Level::Info, fmt::format(__VA_ARGS__)); }
#define WARN(...) { Volts::Print(Volts::Level::Warning, fmt::format(__VA_ARGS__)); }
#define ERROR(...) { Volts::Print(Volts::Level::Error, fmt::format(__VA_ARGS__)); }
#define FATAL(...) { Volts::Print(Volts::Level::Fatal, fmt::format(__VA_ARGS__)); }
