#include "render.h"
#include "backend.h"
#include "support.h"

#include <algorithm>

namespace volts::rsx
{
    struct vk : render
    {
        virtual ~vk() override {}

        virtual void preinit(bool shouldDebug) override
        {
            debug = shouldDebug;
            std::vector<std::string> exts = {};
            std::vector<std::string> layers = {};

            {
                uint32_t num = 0;
                const char** names = glfwGetRequiredInstanceExtensions(&num);

                for(uint32_t i = 0; i < num; i++)
                    exts.push_back(names[i]);

                // this layer is required for the debugger to work
                if(debug)
                    exts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

                if(debug)
                    layers.push_back("VK_LAYER_LUNARG_standard_validation");
            }

            // TODO: configure name
            instance = vulkan::instance("vulkan", exts, layers)
                .expect("failed to create vulkan instance", vulkan::err_to_string);

            if(debug)
                debugger = vulkan::addDebugger(instance).expect("failed to install debug messenger");
        }

        virtual void postinit() override
        {
            surface = vulkan::surface(instance, rsx::window())
                .expect("failed to create vulkan surface");
            
            physicalDevices = vulkan::physicalDevices(instance)
                .expect("failed to get physical devices");

            queues = vulkan::queues(physical(), surface)
                .expect("failed to get queue indicies");

            device = vulkan::device(physical(), queues.graphics.value(), { 
                VK_KHR_SWAPCHAIN_EXTENSION_NAME 
            }).expect("failed to create logical device");

            drawLock = vulkan::semaphore(device)
                .expect("failed to create draw semaphore");

            presentLock = vulkan::semaphore(device)
                .expect("failed to create present semaphore");

            pool = vulkan::commandPool(device, queues.graphics.value())
                .expect("failed to create command pool");

            int w, h;
            glfwGetFramebufferSize(rsx::window(), &w, &h);

            std::tie(swap, format, extent) = vulkan::swapchain(device, physical(), surface, { w, h }, queues)
                .expect("failed to create swapchain");

            pass = vulkan::renderpass(device, format)
                .expect("failed to create renderpass");

            buffers = vulkan::framebuffers(device, extent, pass, frameViews)
                .expect("failed to create framebuffers");
        }

        virtual void begin() override
        {

        }

        virtual void end() override
        {

        }

        virtual void cleanup() override
        {
            if(debug)
                vulkan::removeDebugger(instance, debugger);
        }

        virtual std::string_view name() const override { return "vulkan"; }

    private:
        VkInstance instance;
        VkDevice device;
        VkSurfaceKHR surface;
        
        VkCommandPool pool;
        VkSwapchainKHR swap;
        VkFormat format;
        VkExtent2D extent;
        VkRenderPass pass;

        std::vector<VkImage> frames;
        std::vector<VkImageView> frameViews;
        std::vector<VkFramebuffer> buffers;

        VkSemaphore drawLock;
        VkSemaphore presentLock;


        vulkan::queueIndicies queues;


        std::vector<VkPhysicalDevice> physicalDevices;
        uint32_t physicalIndex = 0;

        VkPhysicalDevice physical() { return physicalDevices[physicalIndex]; }


        bool debug;
        VkDebugUtilsMessengerEXT debugger;
    };

    void vulkan::connect()
    {
        add(new vk());
    }
}