#pragma once

#include <Core/Collections/Array.h>
#include <Core/Collections/CthulhuString.h>

namespace Volts
{
    using Str = Cthulhu::String;

    enum class Severity : Cthulhu::U8
    {
        Trace = 0,
        Info = 1,
        Debug = 2,
        Warning = 3,
        Error = 4,
        Fatal = 5
    };

    struct Log
    {
        Severity LogLevel = Severity::Warning;

        void Info(const Str& Channel, const Str& Message);
        void Trace(const Str& Channel, const Str& Message);
        void Debug(const Str& Channel, const Str& Message);
        void Warning(const Str& Channel, const Str& Message);
        void Error(const Str& Channel, const Str& Message);
        void Fatal(const Str& Channel, const Str& Message);
    
    private:
        void Write(const Str& Channel, Severity S, const Str& Message);
    };

    extern Log Logger;

#   define LOG_INFO(C, Msg) Logger.Info(Msg)
#   define LOG_TRACE(C, Msg) Logger.Trace(Msg);
}