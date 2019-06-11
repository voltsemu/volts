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

    // load in vulkan functions so we can use them
    void LoadFunctions();

    // check if vulkan was found in LoadDLL
    bool Found();

    // load in all extensions
    // returns true if the extensions loaded
    // returns false otherwise
    bool InitExtensions();

    bool Supported(const char* Extension);
}

