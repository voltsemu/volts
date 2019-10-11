#include "Config.h"

#include <Meta/Macros.h>

#include "Core/Logger.h"

#include <string.h>
#include <stdio.h>

#if OS_WINDOWS
#   include <windows.h>
#elif OS_APPLE
#   include <mach-o/dyld.h>
#else
#   include <unistd.h>
#endif

#include <rapidjson/document.h>

namespace Volts::Config
{
    using namespace rapidjson;

    void Load()
    {
    }

    const char* CurrentRender()
    {
#if OS_WINDOWS
        return "DX12";
#elif OS_APPLE
        return "Metal";
#else
        return "Vulkan";
#endif
    }
}