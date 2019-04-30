#pragma once

// we have to use basic types in here for various microsoft related issues

namespace Volts
{
    enum class Level
    {
        Trace,
        Info,
        Debug,
        Warning,
        Error,
        Fatal
    };

    struct EmulatorWindow
    {
        virtual ~EmulatorWindow() {}

        virtual void WriteLog(const char* Channel, Level Severity, const char* Message) = 0;
    };
}

