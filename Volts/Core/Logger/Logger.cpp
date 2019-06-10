#include "Logger.h"
#include <Core/Collections/Array.h>
#include <Core/Collections/CthulhuString.h>
#include <Core/Memory/Memory.h>

#if OS_WINDOWS
#   define _WIN32_DCOM
#   include <comdef.h>
#   include <WbemIdl.h>
#   include <windows.h>
#   include <VersionHelpers.h>
#   pragma comment(lib, "wbemuuid.lib")
#elif OS_APPLE
#   include <CoreServices/CoreServices.h>
#endif

namespace Volts
{
    using namespace Cthulhu;
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

        return String::FromPtr(Data);
#elif OS_APPLE
        SInt32 Major, Minor, BugFix;

        if(Gestalt(gestaltSystemVersionMajor, &Major))
            return "OSX";

        if(Gestalt(gestaltSystemVersionMinor, &Minor))
            return "OSX";

        if(Gestalt(gestaltSystemVersionBugFix, &BugFix))
            return "OSX";

        return "OSX {0}.{1}.{2}"_S.ArrayFormat({
            Utils::ToString((I64)Major),
            Utils::ToString((I64)Minor),
            Utils::ToString((I64)BugFix)
        });
#endif
    }
}