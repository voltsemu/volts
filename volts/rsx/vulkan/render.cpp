#include "render.h"
#include "backend.h"
#include "support.h"

namespace volts::rsx
{
    struct vk : render
    {
        virtual ~vk() override {}

        virtual void preinit() override
        {
            // TODO: configure name
            instance = vulkan::instance("vulkan").expect("failed to create vulkan instance");
            extensions = vulkan::extensions().expect("failed to get instance extensions");

            spdlog::debug("supported extensions");

            for(const auto& extension : extensions)
                spdlog::debug("{} : {}", extension.extensionName, extension.specVersion);

            physicalDevices = vulkan::physicalDevices(instance).expect("failed to get physical extensions");

            spdlog::debug("available devices");

            for(const auto& physicalDevice : physicalDevices)
            {
                VkPhysicalDeviceProperties props = {};
                vkGetPhysicalDeviceProperties(physicalDevice, &props);

                spdlog::debug("{} : {}", props.deviceName, vulkan::to_string(props.deviceType));
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

        virtual std::string_view name() const override { return "vulkan"; }

    private:
        VkInstance instance;

        std::vector<VkExtensionProperties> extensions;
        std::vector<VkPhysicalDevice> physicalDevices;
    };

    void vulkan::connect()
    {
        add(new vk());
    }
}