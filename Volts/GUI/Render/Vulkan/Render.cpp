#include "Render.h"

#define VALIDATE(...) if((__VA_ARGS__)) return false;

namespace Volts::RSX
{
    void Vulkan::Start(GUI::Frame* Handle)
    {

    }

    void Vulkan::Detach()
    {

    }

    bool Vulkan::Setup()
    {
        return true;
    }

    bool Vulkan::CreateInstance()
    {
        VkApplicationInfo AppInfo = {};
        AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        AppInfo.pApplicationName = "Volts";
        AppInfo.pEngineName = "Volts";
        AppInfo.apiVersion = VK_API_VERSION_1_0;
        // TODO: everything
#if 0
        Cthulhu::Array<const char*> Extentions = { VK_KHR_SURFACE_EXTENSION_NAME };

#if OS_WINDOWS
        Extensions += VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#elif OS_LINUX
        Extensions += VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME;
#elif OS_APPLE
        Extensions += VK_MVK_MACOS_SURFACE_EXTENSION_NAME;
#endif

        VkInstanceCreateInfo CreateInfo = {};
        CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        CreateInfo.pNext = nullptr;
        CreateInfo.pApplicationInfo = &AppInfo;

        VALIDATE(vkCreateInstance(&CreateInfo, nullptr, &Instance));
#endif
        return true;
    }
}