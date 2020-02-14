#include "render.h"
#include "backend.h"
#include "support.h"
#include "debug.h"

namespace volts::rsx
{
    VkDebugUtilsMessengerEXT vulkan::debug::utilsMessenger = {};
    
    PFN_vkCreateDebugUtilsMessengerEXT vulkan::debug::vkCreateDebugUtilsMessengerEXT = {};
    PFN_vkDestroyDebugUtilsMessengerEXT vulkan::debug::vkDestroyDebugUtilsMessengerEXT = {};

    struct vk : render
    {
        virtual ~vk() override {}

        virtual void preinit() override
        {
            instance = vulkan::instance();

#if VK_VALIDATE
            vulkan::debug::setup(instance);
#endif
            auto devices = vulkan::phsyical_devices(instance);
            for(auto device : devices)
            {
                VkPhysicalDeviceProperties props = {};
                vkGetPhysicalDeviceProperties(device, &props);
                spdlog::info("device {}", props.deviceName);
            }
        }

        virtual void postinit() override
        {

        }

        virtual void begin() override
        {

        }

        virtual void end() override
        {

        }

        virtual void cleanup() override
        {

        }

    private:
        VkInstance instance;
    };

    void vulkan::connect()
    {
        add(new vk());
    }
}