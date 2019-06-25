#pragma once

#include "Core/Macros.h"
#include <Core/Collections/Array.h>
#include <Core/Memory/Memory.h>
#include "PS3/VM/Render/Render.h"
#include "PS3/VM/Render/Frame.h"

#if OS_WINDOWS
#   define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan/vulkan.h>

namespace Volts::PS3::RSX
{
    struct VulkanDevice : RenderDevice
    {
        VulkanDevice()
            : Device()
        {}

        VulkanDevice(VkPhysicalDevice D);
        virtual std::wstring Name() const override;

        ~VulkanDevice()
        {
            // TODO: memory cleanup
            // delete Properties;
            // Properties = nullptr;
        }

    private:
        VkPhysicalDevice Device;
        VkPhysicalDeviceProperties Properties = {};
    };
}

namespace Volts::PS3::RSX::VulkanSupport
{
    // make an instance thats ready for rendering
    VkInstance MakeInstance();

    // make a simple instance that we can use for querying data
    VkInstance SimpleInstance();

    // load in all extensions
    // returns true if the extensions loaded
    // returns false otherwise
    bool InitExtensions();

    // check if an extension is supported
    bool Supported(const char* Extension);

    Cthulhu::Array<VulkanDevice>* ListDevices(VkInstance Instance);
}

