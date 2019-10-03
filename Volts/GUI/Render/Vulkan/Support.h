#pragma once

#include "Render.h"

#include "Config.h"
#include <vulkan/vulkan.h>

#define VK_VALIDATE(...) if((__VA_ARGS__) != VK_SUCCESS) { exit(1); }

#include <codecvt>
#include <locale>
#include <vector>

namespace Volts::RSX::VulkanSupport
{
    namespace Consts
    {
        constexpr const char* ExtensionNames[] = {
            VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(VK_USE_PLATFORM_WIN32_KHR)
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
            VK_KHR_MACOS_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
            VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
#else
            VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#endif
        };

        constexpr U32 ExtenionCount = sizeof(ExtensionNames) / sizeof(const char*);

#if VVKDEBUG
        constexpr const char* LayerNames[] = {
            "VK_LAYER_LUNARG_standard_validation"
        };

        constexpr U32 LayerCount = sizeof(LayerNames) / sizeof(const char*);
#endif

        constexpr const char* DeviceExtensionNames[] = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        constexpr U32 DeviceExtensionCount = sizeof(DeviceExtensionNames) / sizeof(const char*);
    }

    struct VulkanDevice : Device
    {
        virtual ~VulkanDevice() { }

        VulkanDevice() { }
        VulkanDevice(VkPhysicalDevice Dev)
            : PhysicalDevice(Dev)
        {
            vkGetPhysicalDeviceProperties(PhysicalDevice, &Props);
        }

        virtual const char* Name() const override
        {
            return Props.deviceName;
        }

        VkPhysicalDevice PhysicalDevice;
        VkPhysicalDeviceProperties Props;
    };
}