#define V_REQUIRED_VERSION VK_API_VERSION_1_0
#define V_VERSION VK_MAKE_VERSION(1, 0, 0)

#include "render.h"
#include "backend.h"
#include "support.h"

#include <algorithm>

namespace volts::rsx
{
    struct vulkan : render
    {
        vulkan() 
        { 
#if VK_VALIDATE
            spdlog::debug("vulkan validation is enabled");
#endif
        }

        virtual ~vulkan() override {}

        virtual void preinit(const std::string& name) override
        {
            // we want to use vulkan so we tell glfw to not do any opengl initialization
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

            // create a vulkan instance to use
            {
                VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
                appInfo.apiVersion = V_REQUIRED_VERSION;

                appInfo.pApplicationName = name.c_str();
                // TODO: get game version
                appInfo.applicationVersion = V_VERSION;

                // TODO: configurable extensions, layers, engine name/version
                appInfo.pEngineName = "emulated rsx";
                appInfo.engineVersion = V_VERSION;

                VkInstanceCreateInfo createInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
                createInfo.pApplicationInfo = &appInfo;
                
                // get all the vulkan extensions that glfw requires
                std::vector<const char*> extensions;
                {
                    uint32_t num = 0;
                    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&num);
                
                    for(uint32_t i = 0; i < num; i++)
                        extensions.push_back(glfwExtensions[i]);

#if VK_VALIDATE
                    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
                }

                createInfo.ppEnabledExtensionNames = extensions.data();
                createInfo.enabledExtensionCount = extensions.size();

                std::vector<const char*> layers = {
#if VK_VALIDATE
                    "VK_LAYER_LUNARG_standard_validation"
#endif
                };

                createInfo.ppEnabledLayerNames = layers.data();
                createInfo.enabledLayerCount = layers.size();

                VK_ENSURE(vkCreateInstance(&createInfo, nullptr, &instance));
            }

            // get all physical devices
            physicals = vk::physicalDevices(instance);
        }

        virtual void postinit() override
        {
            // create a surface for the current window
            VK_ENSURE(glfwCreateWindowSurface(instance, rsx::window(), nullptr, &surface));

            // set the current device to the first device
            setDevice(0);
        }

        virtual void begin() override
        {

        }

        virtual void end() override
        {

        }

        virtual void cleanup() override
        {
            vkDestroyInstance(instance, nullptr);
        }

        virtual std::string_view name() const override { return "vulkan"; }
    
    private:
        /// unchanging data
        
        // the toplevel instance
        VkInstance instance;

        // list of all available physical devices
        std::vector<VkPhysicalDevice> physicals;
        
        // the current window surface
        VkSurfaceKHR surface = VK_NULL_HANDLE;

        void setDevice(uint32_t idx)
        {
            device.physical = physicals[idx];
            initQueues();
        }

        /// data for the current device

        // current device data
        struct {
            // the current physical device
            VkPhysicalDevice physical = VK_NULL_HANDLE;

            // the current logical device created from the physical device
            VkDevice logical = VK_NULL_HANDLE;
        } device;

        struct {
            uint32_t graphics = 0;
            uint32_t compute = 0;
            uint32_t transfer = 0;
            uint32_t present = 0;
        } queues;

#define IF_ONCE(expr, ...) if(bool once = true; (expr) && once) { once = false; { __VA_ARGS__ } }

        void initQueues()
        {
            uint32_t num = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(device.physical, &num, nullptr);

            auto* props = VK_SALLOC(VkQueueFamilyProperties, num);
            vkGetPhysicalDeviceQueueFamilyProperties(device.physical, &num, props);

            bool graphics = true,
                 compute = true,
                 transfer = true,
                 present = true;

            for(uint32_t i = 0; i < num; i++)
            {
                if(props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && graphics) 
                {
                    graphics = false;
                    queues.graphics = i;
                }

                if(props[i].queueFlags & VK_QUEUE_COMPUTE_BIT && compute)
                {
                    compute = false;
                    queues.compute = i;
                }

                if(props[i].queueFlags & VK_QUEUE_TRANSFER_BIT && transfer) 
                {
                    transfer = false;
                    queues.transfer = i;
                }

                VkBool32 presentSupport = VK_FALSE;
                vkGetPhysicalDeviceSurfaceSupportKHR(device.physical, i, surface, &presentSupport);
                if(present && presentSupport) 
                {
                    present = false;
                    queues.present = i;
                }
            }

            spdlog::debug("queues g={} c={} t={} p={}", queues.graphics, queues.compute, queues.transfer, queues.present);
        }
    };

    void vk::connect()
    {
        add(new vulkan());
    }
}


#if 0

        virtual void preinit() override
        {
            if(!glslang::InitializeProcess())
            {
                spdlog::critical("failed to initialize glslang");
                std::abort();
            }

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

            std::vector<std::string> exts = {};
            std::vector<std::string> layers = {};

            {
                uint32_t num = 0;
                const char** names = glfwGetRequiredInstanceExtensions(&num);

                for(uint32_t i = 0; i < num; i++)
                    exts.push_back(names[i]);
            }

#if VK_VALIDATE
                // this layer is required for the debugger to work
                exts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

                layers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif

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

            std::tie(pool, buffer) = vulkan::commandPool(device, queues.graphics.value())
                .expect("failed to create command pool");

            int w, h;
            glfwGetFramebufferSize(rsx::window(), &w, &h);

            std::tie(swap, format, extent) = vulkan::swapchain(device, physical, surface, { w, h }, queues)
                .expect("failed to create swapchain");

            pass = vulkan::renderpass(device, format)
                .expect("failed to create renderpass");

            swapImages = vulkan::images(device, swap)
                .expect("failed to get swapchain images");

            swapViews = vulkan::imageViews(device, format, swapImages)
                .expect("failed to create image views");

            swapBuffers = vulkan::framebuffers(device, extent, pass, swapViews)
                .expect("failed to create framebuffers");

            std::tie(depth.image, depth.memory, depth.view) = vulkan::depthStencil(device, physical, VK_FORMAT_D16_UNORM, extent)
                .expect("failed to create depth stencil");

            const char* vert = R"(
                #version 400
                #extension GL_ARB_separate_shader_objects : enable
                #extension GL_ARB_shading_language_420pack : enable
                
                layout (std140, binding = 0) uniform bufferVals { 
                    mat4 mvp; 
                } myBufferVals;

                layout (location = 0) in vec4 pos;
                layout (location = 1) in vec4 colour;
                layout (location = 0) out vec4 ret;

                void main() {
                    ret = colour;
                    gl_Position = myBufferVals.mvp * pos;
                }
            )";

            const char* frag = R"(
                #version 400
                #extension GL_ARB_separate_shader_objects : enable
                #extension GL_ARB_shading_language_420pack : enable
                
                layout (location = 0) in vec4 colour;
                layout (location = 0) out vec4 ret;

                void main() {
                    ret = colour;
                }
            )";
            
            auto shaders = vulkan::modules(device, {
                { vert, "vert", VK_SHADER_STAGE_VERTEX_BIT },
                { frag, "frag", VK_SHADER_STAGE_FRAGMENT_BIT }
            }).expect("failed to create shader modules");

            drawSemaphore = vulkan::semaphore(device).expect("failed to create draw semaphore");
            presentSemaphore = vulkan::semaphore(device).expect("failed to create present semaphore");

            for(uint32_t i = 0; i < swapBuffers.size(); i++)
                fences.push_back(vulkan::fence(device).expect("failed to create fence"));


            {
                using vertex = struct {
                    glm::vec3 pos;
                    glm::vec3 col;
                }

                vertex vertBuffer[] = {
                    { { 1.f, 1.f, 0.f }, { 1.f, 0.f, 0.f } },
                    { { -1.f, 1.f, 0.f }, { 0.f, 1.f, 0.f } },
                    { { 0.f, -1.f, 0.f }, { 0.f, 0.f, 1.f } }
                };
                uint32_t bufferSize = sizeof(verts) / sizeof(vertex);

                uint32_t indexBuffer[] = { 0, 1, 2 };
                verts.count = sizeof(indexBuffer) / sizeof(uint32_t);

                VkMemoryAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
                VkMemoryRequirements reqs;

                using staging = struct {
                    VkDeviceMemory memory;
                    VkBuffer buffer;
                };

                staging stageVerts;
                staging stageIndicies;

                VkBufferCreateInfo bufInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
                bufInfo.size = bufferSize;

                bufInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                
                V_ASSERT(vkCreateBuffer(device, &bufInfo, nullptr, &stageVerts.buffer));
                vkGetBufferMemoryRequirements(device, stageVerts.buffer, &reqs);
                allocInfo.allocationSize = reqs.size;

                // TODO: this could not be more jank
                allocInfo.memoryTypeIndex = 0;
                V_ASSERT(vkAllocateMemory(device, &allocInfo, nullptr, &stageVerts.memory));

                void* data;

                V_ASSERT(vkMapMemory(device, stageVerts.memory, 0, allocInfo.allocationSize, 0, &data));
            }
        }

        virtual void begin() override
        {

        }

        virtual void end() override
        {

        }

        virtual void cleanup() override
        {
            glslang::FinalizeProcess();
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
        VkCommandBuffer buffer;
        VkRenderPass pass;

        VkSemaphore presentSemaphore;
        VkSemaphore drawSemaphore;

        // framedata
        std::vector<VkImage> swapImages;
        std::vector<VkImageView> swapViews;
        std::vector<VkFramebuffer> swapBuffers;
        std::vector<VkFence> fences;

        // depth buffer
        struct {
            VkImage image;
            VkImageView view;
            VkDeviceMemory memory;
        } depth;


        struct {
            VkDeviceMemory memory;
            VkBuffer buffer;
            uint32_t count;
        } verts;

        std::vector<VkPhysicalDevice> physicalDevices;

#if VK_VALIDATE
        VkDebugUtilsMessengerEXT debugger;
#endif

    };

#endif
