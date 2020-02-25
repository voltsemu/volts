#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT

#include "render.h"
#include "backend.h"
#include "support.h"

#include <algorithm>

namespace volts::rsx
{
    struct frameData
    {
        VkImage image;
        VkImageView view;
        VkFramebuffer buffer;
    };

    struct vk : render
    {
        vk()
        {

        }

        virtual ~vk() override {}

        virtual void preinit() override
        {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

            std::vector<std::string> exts = {};
            std::vector<std::string> layers = {};

            {
                uint32_t num = 0;
                const char** names = glfwGetRequiredInstanceExtensions(&num);

                for(uint32_t i = 0; i < num; i++)
                    exts.push_back(names[i]);

#if VK_VALIDATE
                // this layer is required for the debugger to work
                exts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

                layers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif
            }

            // TODO: configure name
            instance = vulkan::instance("vulkan", exts, layers)
                .expect("failed to create vulkan instance", vulkan::err_to_string);

#if VK_VALIDATE
            debugger = vulkan::addDebugger(instance)
                .expect("failed to install debug messenger");
#endif

            physicalDevices = vulkan::physicalDevices(instance)
                .expect("failed to get physical devices");

            physical = physicalDevices[0];
        }

        virtual void postinit() override
        {
            surface = vulkan::surface(instance, rsx::window())
                .expect("failed to create vulkan surface");
            
            queues = vulkan::queues(physical, surface)
                .expect("failed to get queue indicies");

            device = vulkan::device(physical, queues.graphics.value(), { 
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
            }).expect("failed to create logical device");

            pool = vulkan::commandPool(device, queues.graphics.value())
                .expect("failed to create command pool");

            int w, h;
            glfwGetFramebufferSize(rsx::window(), &w, &h);

            std::tie(swap, format, extent) = vulkan::swapchain(device, physical, surface, { w, h }, queues)
                .expect("failed to create swapchain");

            pass = vulkan::renderpass(device, format)
                .expect("failed to create renderpass");

            swapImages = vulkan::images(device, swap)
                .expect("failed to get swapchain images");

            swapViews = vulkan::imageViews(device, format, images)
                .expect("failed to create image views");

            swapBuffers = vulkan::framebuffers(device, extent, pass, views)
                .expect("failed to create framebuffers");

            std::tie(depth.image, depth.memory, depth.view) = vulkan::depthStencil(device, physical, VK_FORMAT_D16_UNORM, extent)
                .expect("failed to create depth stencil");

            

        }

        virtual void begin() override
        {

        }

        virtual void end() override
        {

        }

        virtual void cleanup() override
        {
#if VK_VALIDATE
            vulkan::removeDebugger(instance, debugger);
#endif
        }

        virtual std::string_view name() const override { return "vulkan"; }

    private:
        // global data, ordered by creation
    
        VkInstance instance;
        
        // device data
        vulkan::queueIndicies queues;
        VkPhysicalDevice physical = VK_NULL_HANDLE;
        VkDevice device;

        // presentation data
        VkSurfaceKHR surface;
        VkSwapchainKHR swap;
        VkFormat format;
        VkExtent2D extent;

        // pipeline data
        VkCommandPool pool;
        VkRenderPass pass;


        // framedata
        std::vector<VkImage> swapImages;
        std::vector<VkImageViews> swapViews;
        std::vector<VkFramebuffer> swapBuffers;

        // depth buffer
        struct {
            VkImage image;
            VkImageView view;
            VkDeviceMemory memory;
        } depth;

        VkSemaphore drawLock;
        VkSemaphore presentLock;


        std::vector<VkPhysicalDevice> physicalDevices;

#if VK_VALIDATE
        VkDebugUtilsMessengerEXT debugger;
#endif
    };

    void vulkan::connect()
    {
        add(new vk());
    }
}