#include "VulkanCommon.h"

#include "Core/Logger/Logger.h"

using namespace Cthulhu;

namespace Volts::RSX::Vk
{
    constexpr const char* ExtensionNames[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(VK_USE_PLATFORM_WIN32_KHR)
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
        VK_MVK_MACOS_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
        VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_XCB_KHR)
        VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#endif
    };

    constexpr U32 ExtensionCount = sizeof(ExtensionNames) / sizeof(const char*);

#if VVKDEBUG
    constexpr const char* LayerNames[] = {
        "VK_LAYER_LUNARG_standard_validation"
    };

    constexpr U32 LayerCount = sizeof(LayerNames) / sizeof(const char*);
#endif

    constexpr const char* ApplicationName = "Volts";
    constexpr const char* EngineName = "RSX";


    constexpr const char* DeviceExtensionNames[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    constexpr U32 DeviceExtensionCount = sizeof(DeviceExtensionNames) / sizeof(const char*);


    VkInstance GlobalInstance;
    VkInstance Instance()
    {
        VkApplicationInfo AppInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
        AppInfo.pApplicationName = ApplicationName;
        AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        AppInfo.pEngineName = EngineName;
        AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        AppInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo CreateInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        CreateInfo.pApplicationInfo = &AppInfo;
        CreateInfo.enabledExtensionCount = ExtensionCount;
        CreateInfo.ppEnabledExtensionNames = ExtensionNames;
#if VVKDEBUG
        CreateInfo.enabledLayerCount = LayerCount;
        CreateInfo.ppEnabledLayerNames = LayerNames;
#endif
        VK_VALIDATE(vkCreateInstance(&CreateInfo, nullptr, &GlobalInstance));

        return GlobalInstance;
    }
}