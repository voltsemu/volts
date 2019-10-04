#pragma once

#include "Render.h"

#include "VulkanCommon.h"

namespace Volts::RSX::VulkanSupport
{
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