#pragma once

#include <vulkan/vulkan.h>

namespace fvuk
{
    struct instance
    {
        VkApplicationInfo app = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
    };
}