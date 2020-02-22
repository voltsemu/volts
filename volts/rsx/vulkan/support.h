#include <vulkan/vulkan.h>

#include "volts.h"

#include <platform.h>

#include <result.h>
#include <vector>
#include <string>
#include <optional>

#define VK_ENSURE(expr) { if(VkResult res = (expr); res != VK_SUCCESS) { spdlog::error("vk error {} = {}", #expr, res); } }

namespace volts::rsx::vulkan
{
#define STR_CASE(val) case val: return #val;
    std::string to_string(VkResult res)
    {
        switch(res)
        {
        STR_CASE(VK_SUCCESS);
        STR_CASE(VK_NOT_READY);
        STR_CASE(VK_TIMEOUT);
        STR_CASE(VK_EVENT_SET);
        STR_CASE(VK_EVENT_RESET);
        STR_CASE(VK_INCOMPLETE);
        STR_CASE(VK_ERROR_OUT_OF_HOST_MEMORY);
        STR_CASE(VK_ERROR_OUT_OF_DEVICE_MEMORY);
        STR_CASE(VK_ERROR_INITIALIZATION_FAILED);
        STR_CASE(VK_ERROR_DEVICE_LOST);
        STR_CASE(VK_ERROR_MEMORY_MAP_FAILED);
        STR_CASE(VK_ERROR_LAYER_NOT_PRESENT);
        STR_CASE(VK_ERROR_EXTENSION_NOT_PRESENT);
        STR_CASE(VK_ERROR_FEATURE_NOT_PRESENT);
        STR_CASE(VK_ERROR_INCOMPATIBLE_DRIVER);
        STR_CASE(VK_ERROR_TOO_MANY_OBJECTS);
        STR_CASE(VK_ERROR_FORMAT_NOT_SUPPORTED);
        STR_CASE(VK_ERROR_FRAGMENTED_POOL);
        STR_CASE(VK_ERROR_OUT_OF_POOL_MEMORY);
        STR_CASE(VK_ERROR_INVALID_EXTERNAL_HANDLE);
        STR_CASE(VK_ERROR_SURFACE_LOST_KHR);
        STR_CASE(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
        STR_CASE(VK_SUBOPTIMAL_KHR);
        STR_CASE(VK_ERROR_OUT_OF_DATE_KHR);
        STR_CASE(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
        STR_CASE(VK_ERROR_VALIDATION_FAILED_EXT);
        STR_CASE(VK_ERROR_INVALID_SHADER_NV);
        STR_CASE(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
        STR_CASE(VK_ERROR_NOT_PERMITTED_EXT);
        STR_CASE(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
        STR_CASE(VK_ERROR_FRAGMENTATION_EXT);
        STR_CASE(VK_ERROR_INVALID_DEVICE_ADDRESS_EXT);
        STR_CASE(VK_RESULT_MAX_ENUM);
        default:
            return "VK_ERROR_UNKNOWN";
        }
    }

    std::string to_string(VkPhysicalDeviceType type)
    {
        switch(type)
        {
        STR_CASE(VK_PHYSICAL_DEVICE_TYPE_OTHER);
        STR_CASE(VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
        STR_CASE(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
        STR_CASE(VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU);
        STR_CASE(VK_PHYSICAL_DEVICE_TYPE_CPU);
        STR_CASE(VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM);
        default:
            return "VK_PHYSICAL_DEVICE_TYPE_UNKNOWN";
        }
    }



    svl::result<std::vector<VkPhysicalDevice>, VkResult> physicalDevices(VkInstance instance)
    {
        uint32_t num = 0;

        if(VkResult res = vkEnumeratePhysicalDevices(instance, &num, nullptr); res < 0)
            return svl::err(res);

        std::vector<VkPhysicalDevice> devices(num);
        if(VkResult res = vkEnumeratePhysicalDevices(instance, &num, devices.data()); res < 0)
            return svl::err(res);

        return svl::ok(devices);
    }

    svl::result<std::vector<VkExtensionProperties>, VkResult> extensions(const std::optional<std::string>& layer = std::nullopt)
    {
        const char* name = layer.has_value() ? layer.value().c_str() : nullptr;

        uint32_t num = 0;
        
        if(VkResult res = vkEnumerateInstanceExtensionProperties(name, &num, nullptr); res < 0)
            return svl::err(res);

        std::vector<VkExtensionProperties> props(num);

        if(VkResult res = vkEnumerateInstanceExtensionProperties(name, &num, props.data()); res < 0)
            return svl::err(res);

        return svl::ok(props);
    }

    svl::result<std::vector<VkExtensionProperties>, VkResult> extensions(VkPhysicalDevice device, 
                                                                         const std::optional<std::string>& layer = std::nullopt)
    {
        const char* name = layer.has_value() ? layer.value().c_str() : nullptr;
        uint32_t num = 0;

        if(VkResult res = vkEnumerateDeviceExtensionProperties(device, name, &num, nullptr); res < 0)
            return svl::err(res);

        std::vector<VkExtensionProperties> props(num);
        if(VkResult res = vkEnumerateDeviceExtensionProperties(device, name, &num, props.data()); res < 0)
            return svl::err(res);

        return svl::ok(props);
    }

    svl::result<std::vector<VkLayerProperties>, VkResult> layers(VkPhysicalDevice device)
    {
        uint32_t num = 0;
        if(VkResult res = vkEnumerateDeviceLayerProperties(device, &num, nullptr); res < 0)
            return svl::err(res);

        std::vector<VkLayerProperties> props(num);

        if(VkResult res = vkEnumerateDeviceLayerProperties(device, &num, props.data()); res < 0)
            return svl::err(res);

        return svl::ok(props);
    }

    svl::result<std::vector<VkLayerProperties>, VkResult> layers()
    {
        uint32_t num = 0;
        if(VkResult res = vkEnumerateInstanceLayerProperties(&num, nullptr); res < 0)
            return svl::err(res);

        std::vector<VkLayerProperties> props(num);

        if(VkResult res = vkEnumerateInstanceLayerProperties(&num, props.data()); res < 0)
            return svl::err(res);

        return svl::ok(props);
    }

    svl::result<VkInstance, VkResult> instance(const std::string& name,
                                               VkAllocationCallbacks* allocator = VK_NULL_HANDLE,
                                               const std::vector<std::string>& extensions = {},
                                               const std::vector<std::string>& layers = {})
    {
        // create application info
        VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
        
        // for now we only use the minimum version
        appInfo.apiVersion = VK_API_VERSION_1_0;
        
        // set the application name and version
        appInfo.pApplicationName = name.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

        // set the engine name and version
        appInfo.pEngineName = "emulated rsx";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);


        // setup the create info struct
        VkInstanceCreateInfo createInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };

        createInfo.pApplicationInfo = &appInfo;


        // create char** arrays of layer and extension names
        const char** layerNames = (const char**)salloc(sizeof(char*) * layers.size());
        const char** extensionNames = (const char**)salloc(sizeof(char*) * extensions.size());

        for(int i = 0; i < extensions.size(); i++)
            extensionNames[i] = extensions[i].c_str();

        for(int i = 0; i < layers.size(); i++)
            layerNames[i] = layers[i].c_str();

        createInfo.ppEnabledExtensionNames = extensionNames;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());

        createInfo.ppEnabledLayerNames = layerNames;
        createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());

        // create the instance
        VkInstance out;
        if(VkResult res = vkCreateInstance(&createInfo, allocator, &out); res < 0)
            return svl::err(res);

        spdlog::debug("created vulkan instance for {}", name);

        return svl::ok(out);
    }
}