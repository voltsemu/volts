#include "Config.h"

#include "Core/Logger/Logger.h"

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
    using namespace Cthulhu;
    using namespace rapidjson;

    Document ConfigData;

    std::string ConfigPath()
    {
        char Path[1024] = {};
#if OS_WINDOWS
        GetModuleFileName(nullptr, Path, sizeof(Path));
#elif OS_APPLE
        U32 _;
        _NSGetExecutablePath(Path, &_);
#else
        readlink("/proc/self/exe", Path, sizeof(Path));
#endif

        return fmt::format("{}/Data/config.json", Path);
    }

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