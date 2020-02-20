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

            device = physical().device<VK_QUEUE_GRAPHICS_BIT>();
            auto format = physical().best_format();

            vkGetDeviceQueue(device, physical().queue_index(VK_QUEUE_GRAPHICS_BIT).value(), 0, &queue);
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
        
        using idx_type = typename std::vector<physical_device>::size_type;

        idx_type device_index = 0;
        std::vector<physical_device> devices;

        const physical_device& physical() const { return devices[device_index]; }

        VkDevice device;
        VkQueue queue;
    };

    void vulkan::connect()
    {
        add(new vk());
    }
}