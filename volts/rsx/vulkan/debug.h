#pragma once

#include <vulkan/vulkan.h>

namespace volts::rsx::vulkan::debug
{
    VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
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

    extern VkDebugUtilsMessengerEXT utilsMessenger;

    extern PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
    extern PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;

    void setup(const VkInstance& instance, PFN_vkDebugUtilsMessengerCallbackEXT callback = debug_callback)
    {
        vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
        vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

        VkDebugUtilsMessengerCreateInfoEXT messenger = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        messenger.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        messenger.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        messenger.pfnUserCallback = debug_callback;

        VK_ENSURE(vkCreateDebugUtilsMessengerEXT(instance, &messenger, nullptr, &utilsMessenger));

        spdlog::info("installed debug messaging");
    }

    void cleanup(const VkInstance& instance)
    {
        if(utilsMessenger != VK_NULL_HANDLE)
            vkDestroyDebugUtilsMessengerEXT(instance, utilsMessenger, nullptr);
    }
}