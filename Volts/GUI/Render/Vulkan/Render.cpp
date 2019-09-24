#include "Render.h"

namespace Volts::RSX
{
    Vulkan::Vulkan()
    {
        CreateInstance();
        QueryDevices();
    }

    Vulkan::~Vulkan()
    {
        delete[] RenderDevices;
    }

    void Vulkan::Attach(GUI::Frame* Handle)
    {
        Frame = Handle;
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

    Device* Vulkan::Devices(U32* Count)
    {
        *Count = DeviceCount;
        return RenderDevices;
    }

    void Vulkan::CreateInstance()
    {
        VkApplicationInfo AppInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
        AppInfo.pApplicationName = "Volts";
        AppInfo.pEngineName = "RSX";
        AppInfo.apiVersion = VK_MAKE_VERSION(1, 0, 3);

        VkInstanceCreateInfo CreateInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        CreateInfo.pApplicationInfo = &AppInfo;

        const char* Extensions[] = {
            VK_KHR_SURFACE_EXTENSION_NAME,
#if OS_WINDOWS
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif OS_LINUX
            VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#elif OS_APPLE
            VK_EXT_metal_surface
#endif
        };

        CreateInfo.enabledExtensionCount = 2;
        CreateInfo.ppEnabledExtensionNames = Extensions;

        VK_VALIDATE(vkCreateInstance(&CreateInfo, nullptr, &Instance));
    }

    void Vulkan::QueryDevices()
    {
        vkEnumeratePhysicalDevices(Instance, &DeviceCount, nullptr);
        VkPhysicalDevice* PhysicalDevices = new VkPhysicalDevice[DeviceCount];
        RenderDevices = new VulkanSupport::VulkanDevice[DeviceCount];
        vkEnumeratePhysicalDevices(Instance, &DeviceCount, PhysicalDevices);

        for(U32 I = 0; I < DeviceCount; I++)
        {
            RenderDevices[I] = VulkanSupport::VulkanDevice(PhysicalDevices[I]);
        }

        delete[] PhysicalDevices;
    }
}