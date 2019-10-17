#pragma once

#include "Core/Emulator.h"

#include <vulkan/vulkan.h>

namespace Volts::Render
{
    struct VulkanDevice : Device
    {
        VulkanDevice() {}
        VulkanDevice(VkPhysicalDevice Dev)
            : PhysicalDevice(Dev)
        {}

        VkPhysicalDevice PhysicalDevice = {};
    };
}