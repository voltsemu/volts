#include "Logger.h"
#include <Core/Collections/Array.h>
#include <Core/Collections/CthulhuString.h>

namespace Volts
{
    Level LogLevel = Level::Warning;

    void PrintPrefix(Level S)
    {
        switch(S)
        {
        case Level::Trace:
            printf("trace: ");
            break;
        case Level::Info:
            printf("info: ");
            break;
        case Level::Debug:
            printf("debug: ");
            break;
        case Level::Warning:
            printf("warning: ");
            break;
        case Level::Error:
            printf("error: ");
            break;
        case Level::Fatal:
            printf("fatal: ");
            break;
        default:
            printf("other: ");
            break;
        }
    }
}