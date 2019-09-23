#include "Render.h"

#define VALIDATE(...) if((__VA_ARGS__)) return false;

namespace Volts::RSX
{
    void Vulkan::Attach(GUI::Frame* Handle)
    {
        CreateInstance();
    }

    void Vulkan::Detach()
    {

    }

    void Vulkan::BeginRender()
    {

    }

    void Vulkan::PresentRender()
    {

    }

    void Vulkan::Resize(GUI::Size NewSize)
    {

    }

    void Vulkan::Windowed()
    {

    }

    void Vulkan::Borderless()
    {

    }

    void Vulkan::Fullscreen()
    {

    }

    void Vulkan::CreateInstance()
    {
        VkApplicationInfo Info = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
        Info.pApplicationName = "Volts";
        Info.pEngineName = "RSX";
        Info.apiVersion = 1;

        VkInstanceCreateInfo CreateInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        CreateInfo.pApplicationInfo = &Info;

        const char* Extensions[] = {
            VK_KHR_SURFACE_EXTENSION_NAME,
#if OS_WINDOWS
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif OS_LINUX
            VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
#elif OS_APPLE
            VK_MVK_MACOS_SURFACE_EXTENSION_NAME,
#endif

#if VVKDEBUG
            VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#endif
        };
        CreateInfo.ppEnabledExtensionNames = Extensions;
        CreateInfo.enabledExtensionCount = sizeof(Extensions) / sizeof(const char*);

#if VVKDEBUG
        const char* Layers[] = { "VK_LAYER_LUNARG_standard_validation" };
        CreateInfo.ppEnabledLayerNames = Layers;
        CreateInfo.enabledLayerCount = 1;
#endif

        VK_VALIDATE(vkCreateInstance(&CreateInfo, nullptr, &Instance));

        U32 GPUCount = 0;
        VK_VALIDATE(vkEnumeratePhysicalDevices(Instance, &GPUCount, nullptr));

        VkPhysicalDevice* Devices = new VkPhysicalDevice[GPUCount];
        VK_VALIDATE(vkEnumeratePhysicalDevices(Instance, &GPUCount, Devices));

        for(U32 I = 0; I < GPUCount; I++)
        {

        }
    }
}