#include <vulkan/vulkan.h>

#define VK_ENSURE(...) { if(VkResult res = (__VA_ARGS__); res != VK_SUCCESS) { spdlog::error("vk error {} {}", #__VA_ARG__, res); } }

#include <vector>
#include <string>

namespace fvuk
{
    std::vector<VkLayerProperties> supportedLayers()
    {
        int num = 0;
        vkEnumerateInstanceLayerProperties(&num, nullptr);

        std::vector<Vk
    }
}