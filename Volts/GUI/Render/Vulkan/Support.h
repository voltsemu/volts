#pragma once

#include "Render.h"

#include "Config.h"
#include <vulkan/vulkan.h>

#define VK_VALIDATE(...) if((__VA_ARGS__) != VK_SUCCESS) { exit(1); }

#include <codecvt>
#include <locale>

namespace Volts::RSX::VulkanSupport
{
    struct SwapChain
    {

    };

    struct VulkanDevice : Device
    {
        VulkanDevice() {}
        VulkanDevice(VkPhysicalDevice Dev)
        {
            Handle = Dev;
            vkGetPhysicalDeviceProperties(Handle, &Props);
        }

        virtual std::wstring Name() const override
        {
            return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().from_bytes(Props.deviceName);
        }
    private:
        VkPhysicalDevice Handle;
        VkPhysicalDeviceProperties Props;
    };
}