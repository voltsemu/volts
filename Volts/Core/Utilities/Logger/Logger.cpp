#include "Logger.h"

namespace Volts
{
    namespace
    {
        static const String TPrefix = "trace: ";
        static const String IPrefix = "info: ";
        static const String DPrefix = "debug: ";
        static const String WPrefix = "warning: ";
        static const String EPrefix = "error: ";
        static const String FPrefix = "fatal: ";
    }

    const String& Prefix(Severity S)
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

    void Log::Write(Severity S, const Str& Message)
    {
        if(S >= LogLevel)
        {
#if OS_APPLE || OS_LINUX
            printf("%s\n", (Prefix + Message).CStr());
#elif OS_WINDOWS
            // do something else
#endif
        }
    }

    void Log::Trace(const Str& Message)
    {
        Write(Severity::Trace, Message);
    }

    void Log::Info(const Str& Message)
    {
        Write(Severity::Info, Message);
    }

    void Log::Debug(const Str& Message)
    {
        Write(Severity::Debug, Message);
    }

    void Log::Warning(const Str& Message)
    {
        Write(Severity::Warning, Message);
    }

    void Log::Error(const Str& Message)
    {
        Write(Severity::Error, Message);
    }

    void Log::Fatal(const Str& Message)
    {
        Write(Severity::Fatal, Message);
    }

    Log Logger = Log();
}