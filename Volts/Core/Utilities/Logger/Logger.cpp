#include "Logger.h"

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

    void Write(const char* Channel, Level S, const char* Message, ...)
    {
        va_list Args;
        va_start(Args, Message);
        
        if(S >= LogLevel)
        {
            PrintPrefix(S);
        
            printf("[%s] ", Channel);
            printf(Message, Args);
            printf("\n");
        }

        va_end(Args);
    }
}