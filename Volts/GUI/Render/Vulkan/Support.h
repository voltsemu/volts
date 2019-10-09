#pragma once

#include "Render.h"
#include "Config.h"

#include <vulkan/vulkan.h>

#define VK_VALIDATE(...) { if(auto Err = (__VA_ARGS__); Err != VK_SUCCESS) { MessageBox(nullptr, fmt::format("Err {}", Err).c_str(), "Vulkan error", 0); exit(25); } }

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