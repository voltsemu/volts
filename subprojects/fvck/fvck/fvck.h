#pragma once

#ifndef FVCK_TARGET_WIN32
#   define FVCK_TARGET_WIN32 0
#endif

#ifndef FVCK_TARGET_MACOS
#   define FVCK_TARGET_MACOS 0
#endif

#ifndef FVCK_TARGET_IOS
#   define FVCK_TARGET_IOS 0
#endif

#ifndef FVCK_TARGET_WAYLAND
#   define FVCK_TARGET_WAYLAND 0
#endif

#ifndef FVCK_TARGET_XLIB
#   define FVCK_TARGET_XLIB 0
#endif

// if a user doesnt define a target assume glfw
// because it does everything for us
#if !FVCK_TARGET_WIN32 && !FVCK_TARGET_MACOS && !FVCK_TARGET_IOS && !FVCK_TARGET_WAYLAND && !FVCK_TARGET_XLIB && !FVCK_TARGET_GLFW
#   define FVCK_TARGET_GLFW 1
#endif

#ifndef FVCK_TARGET_GLFW
#   define FVCK_TARGET_GLFW 0
#endif

#include <vulkan/vulkan.h>

#if FVCK_TARGET_GLFW
#   define GLFW_INCLUDE_VULKAN
#   include <GLFW/glfw3.h>
#   include <GLFW/glfw3native.h>
#endif

#include <vector>
#include <string>
#include <string_view>

#ifndef FVCK_MIN_VERSION 
#   define FVCK_MIN_VERSION VK_API_VERSION_1_0
#endif

#define FVCK_ENSURE(...) { if(VkResult res = (__VA_ARGS__); res) { spdlog::error("[{}:{}] = {}", __FILE__, __LINE__, volts::rsx::vk::to_string(res)); } }

namespace fvck
{

#define FVCK_CASE(val) case val: return #val;
    
    std::string_view to_string(VkResult res)
    {
        switch(res)
        {
        FVCK_CASE(VK_SUCCESS);
        FVCK_CASE(VK_NOT_READY);
        FVCK_CASE(VK_TIMEOUT);
        FVCK_CASE(VK_EVENT_SET);
        FVCK_CASE(VK_EVENT_RESET);
        FVCK_CASE(VK_INCOMPLETE);
        FVCK_CASE(VK_ERROR_OUT_OF_HOST_MEMORY);
        FVCK_CASE(VK_ERROR_OUT_OF_DEVICE_MEMORY);
        FVCK_CASE(VK_ERROR_INITIALIZATION_FAILED);
        FVCK_CASE(VK_ERROR_DEVICE_LOST);
        FVCK_CASE(VK_ERROR_MEMORY_MAP_FAILED);
        FVCK_CASE(VK_ERROR_LAYER_NOT_PRESENT);
        FVCK_CASE(VK_ERROR_EXTENSION_NOT_PRESENT);
        FVCK_CASE(VK_ERROR_FEATURE_NOT_PRESENT);
        FVCK_CASE(VK_ERROR_INCOMPATIBLE_DRIVER);
        FVCK_CASE(VK_ERROR_TOO_MANY_OBJECTS);
        FVCK_CASE(VK_ERROR_FORMAT_NOT_SUPPORTED);
        FVCK_CASE(VK_ERROR_FRAGMENTED_POOL);
        FVCK_CASE(VK_ERROR_OUT_OF_POOL_MEMORY);
        FVCK_CASE(VK_ERROR_INVALID_EXTERNAL_HANDLE);
        FVCK_CASE(VK_ERROR_SURFACE_LOST_KHR);
        FVCK_CASE(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
        FVCK_CASE(VK_SUBOPTIMAL_KHR);
        FVCK_CASE(VK_ERROR_OUT_OF_DATE_KHR);
        FVCK_CASE(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
        FVCK_CASE(VK_ERROR_VALIDATION_FAILED_EXT);
        FVCK_CASE(VK_ERROR_INVALID_SHADER_NV);
        FVCK_CASE(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
        FVCK_CASE(VK_ERROR_NOT_PERMITTED_EXT);
        FVCK_CASE(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
        FVCK_CASE(VK_ERROR_FRAGMENTATION_EXT);
        FVCK_CASE(VK_ERROR_INVALID_DEVICE_ADDRESS_EXT);
        FVCK_CASE(VK_RESULT_MAX_ENUM);
        default:
            return "VK_ERROR_UNKNOWN";
        }
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL debugMessengerCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* data,
        void* userdata
    )
    {
        if(severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
            spdlog::debug("vk debug [{}:{}] {}", data->messageIdNumber, data->pMessageIdName, data->pMessage);
        else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
            spdlog::info("vk info [{}:{}] {}", data->messageIdNumber, data->pMessageIdName, data->pMessage);
        else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
            spdlog::warn("vk warning [{}:{}] {}", data->messageIdNumber, data->pMessageIdName, data->pMessage);
        else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
            spdlog::error("vk error [{}:{}] {}", data->messageIdNumber, data->pMessageIdName, data->pMessage);
        else
            spdlog::critical("vk critical [{}:{}] {}", data->messageIdNumber, data->pMessageIdName, data->pMessage);

        return VK_FALSE;
    }

#undef FVCK_CAST

    struct PhysicalDevice
    {
        PhysicalDevice(VkPhysicalDevice device)
            : physical(device)
        {}

    private:
        VkPhysicalDevice physical;
    };

    struct Instance
    {
        Instance(
            const std::string& appName = "",
            const std::vector<std::string>& extensions = {},
            const std::vector<std::string>& layers = {},
            uint32_t appVersion = VK_MAKE_VERSION(1, 0, 0)
        )
        {

            std::vector<const char*> extensionNames;

            for(auto ext : extensions)
                extensionNames.push_back(ext.c_str());

            // add the glfw required extensions
    #if FVCK_TARGET_GLFW
            {
                uint32_t num = 0;
                const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&num);

                for(uint32_t i = 0; i < num; i++)
                    extensionNames.push_back(glfwExtensions[i]);
            }
    #endif

            std::vector<const char*> layerNames;

            for(auto layer : layers)
                layerNames.push_back(layer.c_str());

            
            VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };

            appInfo.pApplicationName = appName.c_str();
            appInfo.applicationVersion = appVersion;

            appInfo.pEngineName = "fvck";
            appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

            VkInstanceCreateInfo createInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
            createInfo.pApplicationInfo = &appInfo;

            createInfo.ppEnabledExtensionNames = extensionNames.data();
            createInfo.enabledExtensionCount = extensionNames.size();

            createInfo.ppEnabledLayerNames = layerNames.data();
            createInfo.enabledLayerCount = layerNames.size();

            FVCK_ENSURE(vkCreateInstance(&createInfo, nullptr, &instance));
        }

        std::vector<PhysicalDevice> devices() const
        {
            uint32_t num = 0;
            FVCK_ENSURE(vkEnumeratePhysicalDevices(instance, &num, nullptr));

            std::vector<VkPhysicalDevice> physicals(num);
            FVCK_ENSURE(vkEnumeratePhysicalDevices(instance, &num, physicals.data()));

            std::vector<PhysicalDevice> out;
            for(auto physical : physicals)
                out.push_back(physical);

            return out;
        }

        VkDebugUtilsMessengerEXT debugMessenger(PFN_vkDebugUtilsMessengerCallbackEXT callback = debugMessengerCallback)
        {
            VkDebugUtilsMessengerCreateInfoEXT createInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };

            createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
            createInfo.pfnUserCallback = callback;

            VkDebugUtilsMessengerEXT out;

            FVCK_ENSURE(vkCreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &out));

            return out;
        }

    private:
        VkInstance instance;
    };

    std::vector<VkLayerProperties> layers()
    {
        uint32_t num = 0;
        vkEnumerateInstanceLayerProperties(&num, nullptr);

        std::vector<VkLayerProperties> props(num);
        vkEnumerateInstanceLayerProperties(&num, props.data());

        return props;
    }

    std::vector<VkExtensionProperties> extensions(const std::string& layer = "")
    {
        const char* layerName = layer.empty() ? nullptr : layer.c_str();
        
        uint32_t num = 0;
        vkEnumerateInstanceExtensionProperties(layerName, &num, nullptr);

        std::vector<VkExtensionProperties> props(num);
        vkEnumerateInstanceExtensionProperties(layerName, &num, props.data());
        
        return props;
    }
}