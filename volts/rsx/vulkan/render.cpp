#include "render.h"
#include "backend.h"
#include "support.h"

#if VK_VALIDATE
#   include "debug.h"
#endif

namespace volts::rsx
{
#if VK_VALIDATE
    VkDebugUtilsMessengerEXT vulkan::debug::utilsMessenger = {};
    
    PFN_vkCreateDebugUtilsMessengerEXT vulkan::debug::vkCreateDebugUtilsMessengerEXT = {};
    PFN_vkDestroyDebugUtilsMessengerEXT vulkan::debug::vkDestroyDebugUtilsMessengerEXT = {};
#endif

    struct vk : render
    {
        virtual ~vk() override {}

        virtual void preinit() override
        {
            instance = vulkan::instance();

#if VK_VALIDATE
            vulkan::debug::setup(instance);
#endif
            devices = vulkan::phsyical_devices(instance);

            for(auto device : devices)
                spdlog::info("device {}", device.name());

            physical = devices[0];
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
        std::vector<physical_device> devices;

        physical_device physical;
    };

    void vulkan::connect()
    {
        add(new vk());
    }
}