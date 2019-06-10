#if WITH_VULKAN

#include <vulkan/vulkan.h>
#include "Render.h"

namespace Volts::PS3::RSX
{
    void Vulkan::Test()
    {
        auto vkCreateInstance = GetProcAddress(VulkanDLL, "vkCreateInstance");
    }

    void Vulkan::Init()
    {
#if OS_WINDOWS
        VulkanDLL = LoadLibraryA("vulakn-1.dll");
#else
        VulkanDLL = dlopen("libvulkan.so", RTLD_NOW);
#endif
    }
}

#endif // WITH_VULKAN