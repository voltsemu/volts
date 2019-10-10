#include "Logger.h"
#include <Core/Collections/Array.h>
#include <Core/Collections/CthulhuString.h>
#include <Core/Memory/Memory.h>

#include <cstdarg>

#include <fmt/format.h>

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

#include "GUI/Frame.h"

namespace Volts
{
    using namespace Cthulhu;

    void Print(Level L, std::string Msg)
    {
        GUI::Frame::Singleton->Log(L, (Msg + "\n").c_str());
    }
}