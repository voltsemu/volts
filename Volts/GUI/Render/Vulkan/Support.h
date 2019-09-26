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
    struct VulkanDevice : Device
    {
        virtual ~VulkanDevice() { }

        VulkanDevice() { }

        virtual std::wstring Name() const override
        {
            return std::wstring();
        }
    };
}