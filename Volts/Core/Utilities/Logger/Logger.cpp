#include "Logger.h"

#include "Volts/Core/Volts.h"

namespace Volts
{
    Level Log::LogLevel = Level::Warning;
    namespace
    {
        static const Str TPrefix = "trace: ";
        static const Str IPrefix = "info: ";
        static const Str DPrefix = "debug: ";
        static const Str WPrefix = "warning: ";
        static const Str EPrefix = "error: ";
        static const Str FPrefix = "fatal: ";
    }

    const Str& Prefix(Level S)
    {
        switch(S)
        {
            default:
            case Level::Trace: return TPrefix;
            case Level::Info: return IPrefix;
            case Level::Debug: return DPrefix;
            case Level::Warning: return WPrefix;
            case Level::Error: return EPrefix;
            case Level::Fatal: return FPrefix;
        }
    }

    void Log::Write(const Str& Channel, Level S, const Str& Message)
    {
        if(S >= LogLevel)
        {
            Volts::Window->WriteLog(Channel.CStr(), S, Message.CStr());
        }
    }

    void Log::Trace(const Str& Channel, const Str& Message)
    {
        Write(Channel, Level::Trace, Message);
    }

    void Log::Info(const Str& Channel, const Str& Message)
    {
        Write(Channel, Level::Info, Message);
    }

    void Log::Debug(const Str& Channel, const Str& Message)
    {
        Write(Channel, Level::Debug, Message);
    }

    void Log::Warning(const Str& Channel, const Str& Message)
    {
        Write(Channel, Level::Warning, Message);
    }

    void Log::Error(const Str& Channel, const Str& Message)
    {
        Write(Channel, Level::Error, Message);
    }

    void Log::Fatal(const Str& Channel, const Str& Message)
    {
        Write(Channel, Level::Fatal, Message);
    }
}