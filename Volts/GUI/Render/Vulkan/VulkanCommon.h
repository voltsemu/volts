#pragma once

#include "Config.h"
#include <vulkan/vulkan.h>

#define VK_VALIDATE(...) if((__VA_ARGS__) != VK_SUCCESS) { exit(1); }

namespace Volts::RSX::Vk
{
    bool Load();
    void Unload();

    VkInstance Instance();
}