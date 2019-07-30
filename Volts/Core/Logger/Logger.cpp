#include "Logger.h"
#include <Core/Collections/Array.h>
#include <Core/Collections/CthulhuString.h>
#include <Core/Memory/Memory.h>

#include <cstdarg>

#if OS_WINDOWS
#   define _WIN32_DCOM
#   include <comdef.h>
#   include <WbemIdl.h>
#   include <windows.h>
#   include <VersionHelpers.h>
#   pragma comment(lib, "wbemuuid.lib")
#elif OS_APPLE
#   include "Core/Darwin/Darwin.h"
#endif

namespace Volts
{
    static auto* Out = fopen("volts.out", "w");

    void Close()
    {
        fclose(Out);
    }

    using namespace Cthulhu;
    Level LogLevel = Level::Warning;

    void PrintPrefix(Level S)
    {
        switch(S)
        {
        case Level::Trace:
            fprintf(Out, "trace: ");
            break;
        case Level::Info:
            fprintf(Out, "info: ");
            break;
        case Level::Debug:
            fprintf(Out, "debug: ");
            break;
        case Level::Warning:
            fprintf(Out, "warning: ");
            break;
        case Level::Error:
            fprintf(Out, "error: ");
            break;
        case Level::Fatal:
            fprintf(Out, "fatal: ");
            break;
        default:
            fprintf(Out, "other: ");
            break;
        }
    }

    void Print(const char* Fmt, ...)
    {
        va_list List;
        va_start(List, Fmt);
        fprintf(Out, Fmt, List);
        va_end(List);
    }

    VideoDriver SupportedDrivers()
    {
        return VideoDriver::None;
    }

    Cthulhu::String GPUDriver()
    {
        return "";
    }

    Cthulhu::String OSName()
    {
#if OS_WINDOWS
        return "Windows (TODO)";
        // this one is going to be a pain to implement as we'll have to
        // bundle the entire emu into its own windows package
        // (which 1. i have no clue how to do and 2. looks really painful)
#elif OS_LINUX
        C8 Data[1024] = {};
        FILE* UName = popen("uname -a", "r");
        if(UName == nullptr)
            return "Unsupported Distro (uname not found)";

        fread(Data, sizeof(C8), sizeof(Data) - 1, UName);
        pclose(UName);

        return String::FromPtr((char*)Data);
#elif OS_APPLE
        return "OSX "_S + String::FromPtr(Darwin::OSName());
#endif
    }
}