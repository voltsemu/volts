#pragma once

#include "Volts/Config.h"

#define FMT_EXCEPTIONS 0
#include <fmt/format.h>

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
}

#define VINFO(...) { Emulator::Get()->Log(Volts::Level::Info, fmt::format(__VA_ARGS__)); }
#define VWARN(...) { Emulator::Get()->Log(Volts::Level::Warning, fmt::format(__VA_ARGS__)); }
#define VERROR(...) { Emulator::Get()->Log(Volts::Level::Error, fmt::format(__VA_ARGS__)); }
#define VFATAL(...) { Emulator::Get()->Log(Volts::Level::Fatal, fmt::format(__VA_ARGS__)); }
