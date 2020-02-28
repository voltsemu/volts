#define V_REQUIRED_VERSION VK_API_VERSION_1_0
#define V_VERSION VK_MAKE_VERSION(1, 0, 0)

#include "render.h"
#include "backend.h"
#include "support.h"

#include <fvck/fvck.h>

#include <algorithm>
#include <limits>

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

            for(auto extension : fvck::extensions())
                spdlog::debug("extension {}:{}", extension.extensionName, extension.specVersion);

            for(auto layer : fvck::layers())
                spdlog::debug("layer {}:{}:{}:{}", layer.layerName, layer.specVersion, layer.implementationVersion, layer.description);

#if VK_VALIDATE
            instance = fvck::Instance(name, { VK_EXT_DEBUG_UTILS_EXTENSION_NAME }, { "VK_LAYER_LUNARG_standard_validation" });
#else
            instance = fvck::Instance(name);
#endif
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
        fvck::Instance instance;

#if VK_VALIDATE
        VkDebugUtilsMessengerEXT messenger;
#endif
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
