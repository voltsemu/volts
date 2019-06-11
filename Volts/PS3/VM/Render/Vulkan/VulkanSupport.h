#pragma once

#include "Core/Macros.h"
#include <Core/Collections/Array.h>

#if OS_WINDOWS
#   define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan/vulkan.h>

namespace Volts::PS3::RSX::VulkanSupport
{
    // load the vulkan DLL in at runtime
    // return true if the library loaded successfully
    // otherwise retrn false
    bool LoadDLL();

    // check if vulkan was found in LoadDLL
    bool Found();

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

    Cthulhu::Array<Cthulhu::String> Devices(VkInstance Instance);
}

