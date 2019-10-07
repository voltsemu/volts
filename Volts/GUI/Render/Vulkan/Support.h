#pragma once

#include "Render.h"
#include "Config.h"

#include <vulkan/vulkan.h>

#define VK_VALIDATE(...) { if((__VA_ARGS__) != VK_SUCCESS) exit(25); }

namespace Volts::RSX::VulkanSupport
{
    struct VulkanDevice : Device
    {
        virtual ~VulkanDevice() {}

        VulkanDevice() {}
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