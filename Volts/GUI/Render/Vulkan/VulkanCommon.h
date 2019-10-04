#pragma once
#include <Meta/Macros.h>
#include "Config.h"

#if OS_APPLE
#   define VK_USE_PLATFORM_MACOS_MVK 1
#endif

#include <vulkan/vulkan.h>

#define VK_VALIDATE(...) if((__VA_ARGS__) != VK_SUCCESS) { exit(1); }

namespace Volts::RSX::Vk
{
    VkInstance Instance();
}