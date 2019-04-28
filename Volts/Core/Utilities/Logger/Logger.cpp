#include "Logger.h"

namespace Volts
{
    namespace
    {
        static const Str TPrefix = "trace: ";
        static const Str IPrefix = "info: ";
        static const Str DPrefix = "debug: ";
        static const Str WPrefix = "warning: ";
        static const Str EPrefix = "error: ";
        static const Str FPrefix = "fatal: ";
    }

    const Str& Prefix(Severity S)
    {
        switch(S)
        {
            default:
            case Severity::Trace: return TPrefix;
            case Severity::Info: return IPrefix;
            case Severity::Debug: return DPrefix;
            case Severity::Warning: return WPrefix;
            case Severity::Error: return EPrefix;
            case Severity::Fatal: return FPrefix;
        }
    }

    void Log::Write(const Str& Channel, Severity S, const Str& Message)
    {
        if(S >= LogLevel)
        {
#if OS_APPLE || OS_LINUX
            printf("%s\n", (Prefix(S) + Channel + Str(" ") + Message).CStr());
#elif OS_WINDOWS
            // do something else
#endif
        }
    }

    void Log::Trace(const Str& Channel, const Str& Message)
    {
        Write(Channel, Severity::Trace, Message);
    }

    void Log::Info(const Str& Channel, const Str& Message)
    {
        Write(Channel, Severity::Info, Message);
    }

    void Log::Debug(const Str& Channel, const Str& Message)
    {
        Write(Channel, Severity::Debug, Message);
    }

    void Log::Warning(const Str& Channel, const Str& Message)
    {
        Write(Channel, Severity::Warning, Message);
    }

    void Log::Error(const Str& Channel, const Str& Message)
    {
        Write(Channel, Severity::Error, Message);
    }

    void Log::Fatal(const Str& Channel, const Str& Message)
    {
        Write(Channel, Severity::Fatal, Message);
    }

    Log Logger = Log();
}