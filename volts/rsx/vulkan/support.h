#include <vulkan/vulkan.h>

#define VK_ENSURE(...) { if(VkResult res = (__VA_ARGS__); res != VK_SUCCESS) { spdlog::error("vk error {} {}", #__VA_ARG__, res); } }

namespace volts::rsx::vulkan
{
    bool validation_support()
    {
        int count = 0;
        vkEnumerateInstanceLayerProperties(&count, nullptr);

        std::vector<VkLayerProperties> layers(count);
        vkEnumerateInstanceLayerProperties(&count, layers.data());

        for(auto layer : layers)
            if(strcmp(layer.layerName, "VK_LAYER_KHRONOS_validation") == 0)
                return true;

        return false;
    }
}