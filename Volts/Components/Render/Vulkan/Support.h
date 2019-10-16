#pragma once

#include "Core/Emulator.h"

#include <vulkan/vulkan.h>

#if OS_WIN

#elif OS_LINUX

#elif OS_APPLE
#   include <dlfcn.h>
#   define DLL_FILE void*
#   define DLOPEN(Name) dlopen(Name, RTLD_NOW)
#endif

namespace Volts::Render
{
    namespace Vk
    {
        bool Init();
    };

    struct VulkanDevice : Device
    {
        VulkanDevice() {}
        VulkanDevice(VkPhysicalDevice Dev)
            : PhysicalDevice(Dev)
        {}

        VkPhysicalDevice PhysicalDevice = {};
    };
}