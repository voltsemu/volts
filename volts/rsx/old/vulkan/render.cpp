#define V_REQUIRED_VERSION VK_API_VERSION_1_0
#define V_VERSION VK_MAKE_VERSION(1, 0, 0)

#include "render.h"
#include "support.h"

#include <optional>

#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_glfw.h"
#include "imgui/examples/imgui_impl_vulkan.h"

namespace volts::rsx
{
    struct queues
    {
        std::optional<uint32_t> graphics = std::nullopt;
        std::optional<uint32_t> compute = std::nullopt;
        std::optional<uint32_t> transfer = std::nullopt;
        std::optional<uint32_t> present = std::nullopt;
    };

    struct vertex
    {
        glm::vec2 position;
        glm::vec3 colour;

        static VkVertexInputBindingDescription binding()
        {
            VkVertexInputBindingDescription desc = {};
            desc.stride = sizeof(vertex);
            desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return desc;
        }

        static std::vector<VkVertexInputAttributeDescription> attributes()
        {
            return {
                { 0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(vertex, position) },
                { 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex, colour) }
            };
        }
    };

    struct vulkan : render
    {
        virtual ~vulkan() override {}

        virtual void preinit(const game& game) override
        {
            glslang::InitializeProcess();
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

            create_instance(game.title, game.version);

#if VK_VALIDATE
            create_messenger();
#endif

            list_devices();

            select_device();
        }

        virtual void postinit(GLFWwindow* win) override
        {
            window = win;

            create_surface();
            create_device();
            create_queues();

            create_swapchain();
            create_images();
            create_views();

            auto frag = compile(R"(
                #version 450
                #extension GL_ARB_separate_shader_objects : enable

                layout (location = 0) in vec3 frag_colour;

                layout (location = 0) out vec4 colour;

                void main()
                {
                    colour = vec4(frag_colour, 1);
                }
            )", EShLangFragment);

            auto vert = compile(R"(
                #version 450
                #extension GL_ARB_separate_shader_objects : enable

                layout (location = 0) in vec2 position;
                layout (location = 1) in vec3 colour;

                layout (location = 0) out vec3 out_colour;

                void main()
                {
                    gl_Position = vec4(position, 0, 1);
                    out_colour = colour;
                }
            )", EShLangVertex);

            create_layout();
            create_renderpass();

            create_pipeline({
                shader_stage(frag, VK_SHADER_STAGE_FRAGMENT_BIT),
                shader_stage(vert, VK_SHADER_STAGE_VERTEX_BIT)
            });

            create_framebuffers();
            create_pool();

            create_vertex_buffer();

            create_buffers();
            create_locks();

            vkDestroyShaderModule(device, vert, nullptr);
            vkDestroyShaderModule(device, frag, nullptr);

            create_descriptor_pool();

            // imgui stuff

            ImGui_ImplGlfw_InitForVulkan(window, true);

            ImGui_ImplVulkan_InitInfo imgui_info = {};
            imgui_info.Instance = instance;
            imgui_info.PhysicalDevice = physical_device;
            imgui_info.Device = device;
            imgui_info.QueueFamily = queue_families().graphics.value();
            imgui_info.Queue = graphics_queue;
            imgui_info.DescriptorPool = descriptor_pool;
            imgui_info.MinImageCount = image_count;
            imgui_info.ImageCount = image_count;
            imgui_info.CheckVkResultFn = [](auto err) { VK_ENSURE(err) };
            ImGui_ImplVulkan_Init(&imgui_info, renderpass);

            auto temp = buffer();

            ImGui_ImplVulkan_CreateFontsTexture(temp); 

            execute(temp);
        }

        virtual void begin() override
        {
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }

        virtual void end() override
        {
            ImGui::Render();
            VK_ENSURE(vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, present_semaphore, VK_NULL_HANDLE, &current_frame));

            VK_ENSURE(vkWaitForFences(device, 1, &fences[current_frame], VK_TRUE, UINT64_MAX));
            VK_ENSURE(vkResetFences(device, 1, &fences[current_frame]));

            VkPipelineStageFlags mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

            VkSubmitInfo submit = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
            submit.pWaitDstStageMask = &mask;
            submit.pWaitSemaphores = &present_semaphore;
            submit.waitSemaphoreCount = 1;

            submit.pSignalSemaphores = &render_semaphore;
            submit.signalSemaphoreCount = 1;

            submit.pCommandBuffers = &command_buffers[current_frame];
            submit.commandBufferCount = 1;

            VK_ENSURE(vkQueueSubmit(graphics_queue, 1, &submit, fences[current_frame]));

            VkPresentInfoKHR present = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
            present.swapchainCount = 1;
            present.pSwapchains = &swapchain;
            present.pImageIndices = &current_frame;

            present.pWaitSemaphores = &render_semaphore;
            present.waitSemaphoreCount = 1;

            VK_ENSURE(vkQueuePresentKHR(present_queue, &present));
        }

        virtual void cleanup() override
        {
            vkDeviceWaitIdle(device);

            ImGui_ImplVulkan_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();

            glslang::FinalizeProcess();

            vkFreeMemory(device, vert_memory, nullptr);
            vkDestroyBuffer(device, vert_buffer, nullptr);

            for(auto fence : fences)
            {
                vkDestroyFence(device, fence, nullptr);
            }

            vkDestroyDescriptorPool(device, descriptor_pool, nullptr);
            vkDestroyCommandPool(device, command_pool, nullptr);

            for(auto buffer : framebuffers)
            {
                vkDestroyFramebuffer(device, buffer, nullptr);
            }

            vkDestroySemaphore(device, present_semaphore, nullptr);
            vkDestroySemaphore(device, render_semaphore, nullptr);

            vkDestroyPipeline(device, pipeline, nullptr);
            vkDestroyPipelineLayout(device, layout, nullptr);
            vkDestroyRenderPass(device, renderpass, nullptr);

            for(auto view : swapchain_views)
            {
                vkDestroyImageView(device, view, nullptr);
            }

            vkDestroySwapchainKHR(device, swapchain, nullptr);

            vkDestroyDevice(device, nullptr);

            vkDestroySurfaceKHR(instance, surface, nullptr);

#if VK_VALIDATE
            remove_messenger();
#endif
            vkDestroyInstance(instance, nullptr);
        }

        virtual const char* name() const override { return "vulkan"; }

    private:

        GLFWwindow* window;

        using memory = std::tuple<VkBuffer, VkDeviceMemory>;

        VkInstance instance;
        std::vector<VkPhysicalDevice> physical_devices;
        VkPhysicalDevice physical_device;
        VkDevice device;
        VkSurfaceKHR surface;

        VkQueue graphics_queue;
        VkQueue present_queue;

        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        VkSurfaceFormatKHR swapchain_format;
        VkExtent2D swapchain_extent;

        VkPipelineLayout layout;
        VkPipeline pipeline;
        uint32_t image_count = 0;

        VkRenderPass renderpass;
        std::vector<VkImage> swapchain_images;
        std::vector<VkImageView> swapchain_views;
        std::vector<VkFramebuffer> framebuffers;

        VkCommandPool command_pool;
        std::vector<VkCommandBuffer> command_buffers;

        VkSemaphore present_semaphore;
        VkSemaphore render_semaphore;

        std::vector<VkFence> fences;
        uint32_t current_frame = 0;

        const std::vector<vertex> verts = {
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
        };
        VkBuffer vert_buffer;
        VkDeviceMemory vert_memory;


        VkDescriptorPool descriptor_pool;

#if VK_VALIDATE
        VkDebugUtilsMessengerEXT messenger;

        void create_messenger();
        void remove_messenger();
#endif

        void create_instance(const char* title, const char* version);
        void list_devices();
        void select_device();
        void create_device();
        queues queue_families();
        void create_surface();
        void create_queues();
        void create_swapchain();
        void create_images();
        void create_views();
        void create_layout();
        void create_renderpass();

        void create_vertex_buffer();
        std::tuple<VkBuffer, VkDeviceMemory> create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags flags);
        void copy_buffer(VkBuffer dest, VkBuffer source, VkDeviceSize length);

        void create_pipeline(const std::vector<VkPipelineShaderStageCreateInfo>& stages);
        void create_framebuffers();
        void create_pool();
        void create_buffers();
        void create_locks();
        void create_descriptor_pool();

        VkCommandBuffer buffer();
        void execute(VkCommandBuffer buffer);

        uint32_t memory_type(uint32_t filter, VkMemoryPropertyFlags flags);

        VkShaderModule compile(const char* source, EShLanguage lang);
        VkPipelineShaderStageCreateInfo shader_stage(VkShaderModule mod, VkShaderStageFlagBits stage);
    };

    render* vk::connect()
    {
        return new vulkan();
    }

    void vulkan::create_descriptor_pool()
    {
        std::vector<VkDescriptorPoolSize> sizes = {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1024 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1024 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1024 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1024 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1024 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1024 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1024 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1024 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1024 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1024 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1024 }
        };

        VkDescriptorPoolCreateInfo create_info = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
        create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        create_info.maxSets = 1024 * sizes.size();
        create_info.poolSizeCount = sizes.size();
        create_info.pPoolSizes = sizes.data();
        VK_ENSURE(vkCreateDescriptorPool(device, &create_info, nullptr, &descriptor_pool));
    }

    VkCommandBuffer vulkan::buffer()
    {
        VkCommandBufferAllocateInfo alloc_info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        alloc_info.commandPool = command_pool;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = 1;

        VkCommandBuffer out;
        VK_ENSURE(vkAllocateCommandBuffers(device, &alloc_info, &out));

        VkCommandBufferBeginInfo buffer_info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        buffer_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        VK_ENSURE(vkBeginCommandBuffer(out, &buffer_info));

        return out;
    }

    void vulkan::execute(VkCommandBuffer buffer)
    {
        VK_ENSURE(vkEndCommandBuffer(buffer));

        VkSubmitInfo submit = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submit.commandBufferCount = 1;
        submit.pCommandBuffers = &buffer;

        VK_ENSURE(vkQueueSubmit(graphics_queue, 1, &submit, VK_NULL_HANDLE));
        VK_ENSURE(vkQueueWaitIdle(graphics_queue));

        vkFreeCommandBuffers(device, command_pool, 1, &buffer);
    }

    VkPipelineShaderStageCreateInfo vulkan::shader_stage(VkShaderModule mod, VkShaderStageFlagBits stage)
    {
        VkPipelineShaderStageCreateInfo create_info = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
        create_info.stage = stage;
        create_info.module = mod;
        create_info.pName = "main";

        return create_info;
    }

    queues vulkan::queue_families()
    {
        queues out = {};

        uint32_t num = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &num, nullptr);

        std::vector<VkQueueFamilyProperties> props(num);
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &num, props.data());

        for(uint32_t i = 0; i < num; i++)
        {
            if(props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && !out.graphics.has_value())
                out.graphics = i;

            if(props[i].queueFlags & VK_QUEUE_COMPUTE_BIT && !out.compute.has_value())
                out.compute = i;

            if(props[i].queueFlags & VK_QUEUE_TRANSFER_BIT & !out.transfer.has_value())
                out.transfer = i;

            if(!out.present.has_value())
            {
                VkBool32 supports_present = VK_FALSE;
                vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &supports_present);

                if(supports_present)
                    out.present = i;
            }
        }

        return out;
    }

    void vulkan::create_instance(const char* title, const char* version)
    {
        VkApplicationInfo app = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
        app.apiVersion = V_REQUIRED_VERSION;

        app.pEngineName = "emulated rsx";
        app.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        
        app.pApplicationName = title;
        app.applicationVersion =  VK_MAKE_VERSION(version[0] - 48, version[2] - 48, version[3] - 48);

        VkInstanceCreateInfo create = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        create.pApplicationInfo = &app;



        std::vector<const char*> extensions = {
#if VK_VALIDATE
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
        };

        {
            uint32_t num = 0;
            const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&num);

            for(uint32_t i = 0; i < num; i++)
                extensions.push_back(glfwExtensions[i]);
        }



        std::vector<const char*> layers = {
#if VK_VALIDATE
            "VK_LAYER_LUNARG_standard_validation"
#endif
        };



        create.enabledExtensionCount = extensions.size();
        create.ppEnabledExtensionNames = extensions.data();

        create.enabledLayerCount = layers.size();
        create.ppEnabledLayerNames = layers.data();

        VK_ENSURE(vkCreateInstance(&create, nullptr, &instance));

        spdlog::debug("created vulkan instance");
    }

    void vulkan::list_devices()
    {
        uint32_t num = 0;
        VK_ENSURE(vkEnumeratePhysicalDevices(instance, &num, nullptr));

        physical_devices.resize(num);
        VK_ENSURE(vkEnumeratePhysicalDevices(instance, &num, physical_devices.data()));
    }

    void vulkan::select_device()
    {
        // TODO: we'll have a config at some point
        physical_device = physical_devices[0];
    }

    void vulkan::create_device()
    {
        auto families = queue_families();

        float priority = 1.f;

        std::vector<VkDeviceQueueCreateInfo> queue_infos = {};

        for(uint32_t index : std::set<uint32_t>({ 
            families.graphics.value(), 
            families.present.value() 
        }))
        {
            VkDeviceQueueCreateInfo queue_info = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
            queue_info.queueFamilyIndex = index;
            queue_info.queueCount = 1;
            queue_info.pQueuePriorities = &priority;

            queue_infos.push_back(queue_info);
        }

        VkPhysicalDeviceFeatures features = {};

        VkDeviceCreateInfo create_info = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
        create_info.pQueueCreateInfos = queue_infos.data();
        create_info.queueCreateInfoCount = queue_infos.size();

        create_info.pEnabledFeatures = &features;

        std::vector<const char*> extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        create_info.enabledExtensionCount = extensions.size();
        create_info.ppEnabledExtensionNames = extensions.data();

        VK_ENSURE(vkCreateDevice(physical_device, &create_info, nullptr, &device));
    }

    void vulkan::create_surface()
    {
        VK_ENSURE(glfwCreateWindowSurface(instance, window, nullptr, &surface));
    }

    void vulkan::create_queues()
    {
        auto families = queue_families();

        vkGetDeviceQueue(device, families.graphics.value(), 0, &graphics_queue);
        vkGetDeviceQueue(device, families.present.value(), 0, &present_queue);
    }

    VkSurfaceFormatKHR choose_format(const std::vector<VkSurfaceFormatKHR>& formats)
    {
        for(auto format : formats)
            if(format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return format;
        
        return formats[0];
    }

    VkPresentModeKHR choose_mode(const std::vector<VkPresentModeKHR>& modes)
    {
        for(auto mode : modes)
            if(mode == VK_PRESENT_MODE_MAILBOX_KHR)
                return mode;

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D choose_extent(const VkSurfaceCapabilitiesKHR& caps, GLFWwindow* window)
    {
        if(caps.currentExtent.width != UINT32_MAX)
            return caps.currentExtent;

        int w, h;

        glfwGetFramebufferSize(window, &w, &h);

        VkExtent2D extent = { 
            std::clamp<uint32_t>(w, caps.minImageExtent.width, caps.maxImageExtent.width), 
            std::clamp<uint32_t>(h, caps.minImageExtent.height, caps.maxImageExtent.height) 
        };

        return extent;
    }

    void vulkan::create_swapchain()
    {
        VkSurfaceCapabilitiesKHR caps;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &caps);


        uint32_t num_formats = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &num_formats, nullptr);

        std::vector<VkSurfaceFormatKHR> formats(num_formats);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &num_formats, formats.data());


        uint32_t num_modes = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &num_modes, nullptr);
        
        std::vector<VkPresentModeKHR> modes(num_modes);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &num_modes, modes.data());
    
        swapchain_format = choose_format(formats);
        auto mode = choose_mode(modes);
        swapchain_extent = choose_extent(caps, window);

        image_count = std::min(caps.minImageCount ? caps.minImageCount + 1 : 1, caps.maxImageCount);
    
        VkSwapchainCreateInfoKHR create_info = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };

        create_info.surface = surface;
        create_info.minImageCount = image_count;
        create_info.imageFormat = swapchain_format.format;
        create_info.imageColorSpace = swapchain_format.colorSpace;
        create_info.imageExtent = swapchain_extent;
        create_info.imageArrayLayers = 1;
        create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        auto indices = queue_families();
        uint32_t families[] = {indices.graphics.value(), indices.present.value()};

        if(indices.graphics.value() != indices.present.value())
        {
            create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            create_info.queueFamilyIndexCount = 2;
            create_info.pQueueFamilyIndices = families;
        }
        else
        {
            create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        create_info.presentMode = mode;
        create_info.preTransform = caps.currentTransform;
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.clipped = VK_TRUE;

        create_info.oldSwapchain = swapchain;

        VK_ENSURE(vkCreateSwapchainKHR(device, &create_info, nullptr, &swapchain));
    }

    void vulkan::create_images()
    {
        uint32_t num = 0;
        vkGetSwapchainImagesKHR(device, swapchain, &num, nullptr);

        swapchain_images.resize(num);
        vkGetSwapchainImagesKHR(device, swapchain, &num, swapchain_images.data());
    }

    void vulkan::create_views()
    {
        swapchain_views.resize(swapchain_images.size());

        for(uint32_t i = 0; i < swapchain_images.size(); i++)
        {
            VkImageViewCreateInfo create_info = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
            create_info.image = swapchain_images[i];
            create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            create_info.format = swapchain_format.format;

            create_info.components = {
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY
            };

            create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            create_info.subresourceRange.baseMipLevel = 0;
            create_info.subresourceRange.levelCount = 1;
            create_info.subresourceRange.baseArrayLayer = 0;
            create_info.subresourceRange.layerCount = 1;

            VK_ENSURE(vkCreateImageView(device, &create_info, nullptr, &swapchain_views[i]));
        }
    }

    TBuiltInResource default_resources()
    {
        TBuiltInResource res;

        res.maxLights = 32;
        res.maxClipPlanes = 6;
        res.maxTextureUnits = 32;
        res.maxTextureCoords = 32;
        res.maxVertexAttribs = 64;
        res.maxVertexUniformComponents = 4096;
        res.maxVaryingFloats = 64;
        res.maxVertexTextureImageUnits = 32;
        res.maxCombinedTextureImageUnits = 80;
        res.maxTextureImageUnits = 32;
        res.maxFragmentUniformComponents = 4096;
        res.maxDrawBuffers = 32;
        res.maxVertexUniformVectors = 128;
        res.maxVaryingVectors = 8;
        res.maxFragmentUniformVectors = 16;
        res.maxVertexOutputVectors = 16;
        res.maxFragmentInputVectors = 15;
        res.minProgramTexelOffset = -8;
        res.maxProgramTexelOffset = 7;
        res.maxClipDistances = 8;
        res.maxComputeWorkGroupCountX = 65535;
        res.maxComputeWorkGroupCountY = 65535;
        res.maxComputeWorkGroupCountZ = 65535;
        res.maxComputeWorkGroupSizeX = 1024;
        res.maxComputeWorkGroupSizeY = 1024;
        res.maxComputeWorkGroupSizeZ = 64;
        res.maxComputeUniformComponents = 1024;
        res.maxComputeTextureImageUnits = 16;
        res.maxComputeImageUniforms = 8;
        res.maxComputeAtomicCounters = 8;
        res.maxComputeAtomicCounterBuffers = 1;
        res.maxVaryingComponents = 60;
        res.maxVertexOutputComponents = 64;
        res.maxGeometryInputComponents = 64;
        res.maxGeometryOutputComponents = 128;
        res.maxFragmentInputComponents = 128;
        res.maxImageUnits = 8;
        res.maxCombinedImageUnitsAndFragmentOutputs = 8;
        res.maxCombinedShaderOutputResources = 8;
        res.maxImageSamples = 0;
        res.maxVertexImageUniforms = 0;
        res.maxTessControlImageUniforms = 0;
        res.maxTessEvaluationImageUniforms = 0;
        res.maxGeometryImageUniforms = 0;
        res.maxFragmentImageUniforms = 8;
        res.maxCombinedImageUniforms = 8;
        res.maxGeometryTextureImageUnits = 16;
        res.maxGeometryOutputVertices = 256;
        res.maxGeometryTotalOutputComponents = 1024;
        res.maxGeometryUniformComponents = 1024;
        res.maxGeometryVaryingComponents = 64;
        res.maxTessControlInputComponents = 128;
        res.maxTessControlOutputComponents = 128;
        res.maxTessControlTextureImageUnits = 16;
        res.maxTessControlUniformComponents = 1024;
        res.maxTessControlTotalOutputComponents = 4096;
        res.maxTessEvaluationInputComponents = 128;
        res.maxTessEvaluationOutputComponents = 128;
        res.maxTessEvaluationTextureImageUnits = 16;
        res.maxTessEvaluationUniformComponents = 1024;
        res.maxTessPatchComponents = 120;
        res.maxPatchVertices = 32;
        res.maxTessGenLevel = 64;
        res.maxViewports = 16;
        res.maxVertexAtomicCounters = 0;
        res.maxTessControlAtomicCounters = 0;
        res.maxTessEvaluationAtomicCounters = 0;
        res.maxGeometryAtomicCounters = 0;
        res.maxFragmentAtomicCounters = 8;
        res.maxCombinedAtomicCounters = 8;
        res.maxAtomicCounterBindings = 1;
        res.maxVertexAtomicCounterBuffers = 0;
        res.maxTessControlAtomicCounterBuffers = 0;
        res.maxTessEvaluationAtomicCounterBuffers = 0;
        res.maxGeometryAtomicCounterBuffers = 0;
        res.maxFragmentAtomicCounterBuffers = 1;
        res.maxCombinedAtomicCounterBuffers = 1;
        res.maxAtomicCounterBufferSize = 16384;
        res.maxTransformFeedbackBuffers = 4;
        res.maxTransformFeedbackInterleavedComponents = 64;
        res.maxCullDistances = 8;
        res.maxCombinedClipAndCullDistances = 8;
        res.maxSamples = 4;
        res.maxMeshOutputVerticesNV = 256;
        res.maxMeshOutputPrimitivesNV = 512;
        res.maxMeshWorkGroupSizeX_NV = 32;
        res.maxMeshWorkGroupSizeY_NV = 1;
        res.maxMeshWorkGroupSizeZ_NV = 1;
        res.maxTaskWorkGroupSizeX_NV = 32;
        res.maxTaskWorkGroupSizeY_NV = 1;
        res.maxTaskWorkGroupSizeZ_NV = 1;
        res.maxMeshViewCountNV = 4;
        res.limits.nonInductiveForLoops = 1;
        res.limits.whileLoops = 1;
        res.limits.doWhileLoops = 1;
        res.limits.generalUniformIndexing = 1;
        res.limits.generalAttributeMatrixVectorIndexing = 1;
        res.limits.generalVaryingIndexing = 1;
        res.limits.generalSamplerIndexing = 1;
        res.limits.generalVariableIndexing = 1;
        res.limits.generalConstantMatrixVectorIndexing = 1;

        return res;
    }

    VkShaderModule vulkan::compile(const char* source, EShLanguage lang)
    {
        auto rules = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);
        auto resources = default_resources();

        glslang::TShader shader(lang);

        shader.setStrings(&source, 1);

        if(!shader.parse(&resources, 100, false, rules))
        {
            spdlog::error("failed to parse glsl\n{}\n{}",
                shader.getInfoLog(),
                shader.getInfoDebugLog()
            );

            return VK_NULL_HANDLE;
        }

        glslang::TProgram program;
        program.addShader(&shader);

        if(!program.link(rules))
        {
            spdlog::error("failed to link shader\n{}\n{}",
                program.getInfoLog(),
                program.getInfoDebugLog()
            );

            return VK_NULL_HANDLE;
        }

        std::vector<uint32_t> spirv;
        glslang::GlslangToSpv(*program.getIntermediate(lang), spirv);

        VkShaderModuleCreateInfo module_info = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
        module_info.codeSize = spirv.size() * sizeof(uint32_t);
        module_info.pCode = spirv.data();

        VkShaderModule mod;
        VK_ENSURE(vkCreateShaderModule(device, &module_info, nullptr, &mod));

        return mod;
    }

    void vulkan::create_layout()
    {
        VkPipelineLayoutCreateInfo create_info = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        
        VK_ENSURE(vkCreatePipelineLayout(device, &create_info, nullptr, &layout));
    }

    void vulkan::create_renderpass()
    {
        VkAttachmentDescription attach = {};
        attach.format = swapchain_format.format;
        attach.samples = VK_SAMPLE_COUNT_1_BIT;
        attach.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attach.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attach.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attach.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attach.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attach.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference ref = {};
        ref.attachment = 0;
        ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &ref;

        VkRenderPassCreateInfo create_info = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
        create_info.attachmentCount = 1;
        create_info.pAttachments = &attach;
        create_info.subpassCount = 1;
        create_info.pSubpasses = &subpass;

        VK_ENSURE(vkCreateRenderPass(device, &create_info, nullptr, &renderpass));
    }

    void vulkan::create_pipeline(const std::vector<VkPipelineShaderStageCreateInfo>& stages)
    {
        VkPipelineVertexInputStateCreateInfo vert_info = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };

        auto binding = vertex::binding();
        auto attribs = vertex::attributes();

        vert_info.vertexBindingDescriptionCount = 1;
        vert_info.pVertexBindingDescriptions = &binding;

        vert_info.vertexAttributeDescriptionCount = attribs.size();
        vert_info.pVertexAttributeDescriptions = attribs.data();

        VkPipelineInputAssemblyStateCreateInfo assembly = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        assembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport = {};
        viewport.x = 0.f;
        viewport.y = 0.f;
        viewport.width = (float)swapchain_extent.width;
        viewport.height = (float)swapchain_extent.height;
        viewport.minDepth = 0.f;
        viewport.maxDepth = 1.f;

        VkRect2D scissor = {};
        scissor.offset = {0, 0};
        scissor.extent = swapchain_extent;

        VkPipelineViewportStateCreateInfo viewport_state = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        viewport_state.viewportCount = 1;
        viewport_state.pViewports = &viewport;
        viewport_state.scissorCount = 1;
        viewport_state.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo raster = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        raster.depthClampEnable = VK_FALSE;
        raster.rasterizerDiscardEnable = VK_FALSE;
        raster.polygonMode = VK_POLYGON_MODE_FILL;
        raster.lineWidth = 1.0f;
        raster.cullMode = VK_CULL_MODE_BACK_BIT;
        raster.frontFace = VK_FRONT_FACE_CLOCKWISE;
        raster.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo sample = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        sample.sampleShadingEnable = VK_FALSE;
        sample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        sample.minSampleShading = 1.f;

        VkPipelineColorBlendAttachmentState blend = {};
        blend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        blend.blendEnable = VK_FALSE;
        blend.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        blend.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blend.colorBlendOp = VK_BLEND_OP_ADD;
        blend.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        blend.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        blend.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo blending = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        blending.logicOpEnable = VK_FALSE;
        blending.logicOp = VK_LOGIC_OP_COPY;
        blending.attachmentCount = 1;
        blending.pAttachments = &blend;

        VkDynamicState states[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH };
        VkPipelineDynamicStateCreateInfo dynamic_states = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        dynamic_states.dynamicStateCount = 2;
        dynamic_states.pDynamicStates = states;

        VkGraphicsPipelineCreateInfo create_info = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
        create_info.stageCount = stages.size();
        create_info.pStages = stages.data();

        create_info.pVertexInputState = &vert_info;
        create_info.pInputAssemblyState = &assembly;
        create_info.pViewportState = &viewport_state;
        create_info.pRasterizationState = &raster;
        create_info.pMultisampleState = &sample;
        create_info.pColorBlendState = &blending;
        create_info.layout = layout;
        create_info.renderPass = renderpass;
        create_info.subpass = 0;

        VK_ENSURE(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &create_info, nullptr, &pipeline));
    }

    uint32_t vulkan::memory_type(uint32_t filter, VkMemoryPropertyFlags flags)
    {
        VkPhysicalDeviceMemoryProperties props;
        vkGetPhysicalDeviceMemoryProperties(physical_device, &props);

        for(uint32_t i = 0; i < props.memoryTypeCount; i++)
            if(filter & (1 << i) && (props.memoryTypes[i].propertyFlags & flags) == flags)
                return i;

        V_ASSERT(false);
    }

    std::tuple<VkBuffer, VkDeviceMemory> vulkan::create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags flags)
    {
        VkBufferCreateInfo buffer_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        buffer_info.size = size;
        buffer_info.usage = usage;
        buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkBuffer buffer;

        VK_ENSURE(vkCreateBuffer(device, &buffer_info, nullptr, &buffer));

        VkMemoryRequirements reqs;
        vkGetBufferMemoryRequirements(device, buffer, &reqs);

        VkMemoryAllocateInfo alloc_info = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
        alloc_info.allocationSize = reqs.size;
        alloc_info.memoryTypeIndex = memory_type(reqs.memoryTypeBits, flags);

        VkDeviceMemory memory;

        VK_ENSURE(vkAllocateMemory(device, &alloc_info, nullptr, &memory));

        vkBindBufferMemory(device, buffer, memory, 0);

        return { buffer, memory };
    }

    void vulkan::copy_buffer(VkBuffer dest, VkBuffer source, VkDeviceSize length)
    {
        auto buf = buffer();

        VkBufferCopy region = {};
        region.size = length;

        vkCmdCopyBuffer(buf, source, dest, 1, &region);

        execute(buf);
    }

    void vulkan::create_vertex_buffer()
    {
        VkDeviceSize size = verts.size() * sizeof(vertex);

        auto [ staging_buffer, staging_memory ] = create_buffer(
            size, 
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        void* data;
        vkMapMemory(device, staging_memory, 0, size, 0, &data);

        memcpy(data, verts.data(), size);

        vkUnmapMemory(device, staging_memory);

        std::tie(vert_buffer, vert_memory) = create_buffer(
            size, 
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

        copy_buffer(vert_buffer, staging_buffer, size);
        
        vkDestroyBuffer(device, staging_buffer, nullptr);
        vkFreeMemory(device, staging_memory, nullptr);
    }

    void vulkan::create_framebuffers()
    {
        framebuffers.resize(swapchain_images.size());

        for(uint32_t i = 0; i < swapchain_images.size(); i++)
        {
            VkFramebufferCreateInfo create_info = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
            create_info.renderPass = renderpass;
            create_info.attachmentCount = 1;
            create_info.pAttachments = &swapchain_views[i];
            create_info.width = swapchain_extent.width;
            create_info.height = swapchain_extent.height;
            create_info.layers = 1;

            VK_ENSURE(vkCreateFramebuffer(device, &create_info, nullptr, &framebuffers[i]));
        }
    }

    void vulkan::create_pool()
    {
        VkCommandPoolCreateInfo create_info = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        create_info.queueFamilyIndex = queue_families().graphics.value();

        VK_ENSURE(vkCreateCommandPool(device, &create_info, nullptr, &command_pool));
    }

    void vulkan::create_buffers()
    {
        command_buffers.resize(framebuffers.size());

        VkCommandBufferAllocateInfo alloc_info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        alloc_info.commandPool = command_pool;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = command_buffers.size();

        VK_ENSURE(vkAllocateCommandBuffers(device, &alloc_info, command_buffers.data()));

        for(uint32_t i = 0; i < command_buffers.size(); i++)
        {
            VkCommandBufferBeginInfo buffer_info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
            VK_ENSURE(vkBeginCommandBuffer(command_buffers[i], &buffer_info));

            VkRenderPassBeginInfo begin_info = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
            begin_info.renderPass = renderpass;
            begin_info.framebuffer = framebuffers[i];
            begin_info.renderArea.offset = {0, 0};
            begin_info.renderArea.extent = swapchain_extent;

            VkClearValue clear = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
            begin_info.clearValueCount = 1;
            begin_info.pClearValues = &clear;

            vkCmdBeginRenderPass(command_buffers[i], &begin_info, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

            VkDeviceSize offsets[] = { 0 };

            vkCmdBindVertexBuffers(command_buffers[i], 0, 1, &vert_buffer, offsets);

            vkCmdDraw(command_buffers[i], verts.size(), 1, 0, 0);

            vkCmdEndRenderPass(command_buffers[i]);

            VK_ENSURE(vkEndCommandBuffer(command_buffers[i]));
        }
    }

    void vulkan::create_locks()
    {
        fences.resize(swapchain_images.size());

        VkFenceCreateInfo fence_info = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for(auto& fence : fences)
        {
            VK_ENSURE(vkCreateFence(device, &fence_info, nullptr, &fence));
        }

        VkSemaphoreCreateInfo create_info = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

        VK_ENSURE(vkCreateSemaphore(device, &create_info, nullptr, &present_semaphore));
        VK_ENSURE(vkCreateSemaphore(device, &create_info, nullptr, &render_semaphore));
    }

#if VK_VALIDATE
    VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* data,
        void* userdata
    )
    {
        if(severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
            spdlog::debug("[{}:{}] {}", data->messageIdNumber, data->pMessageIdName, data->pMessage);
        else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
            spdlog::info("[{}:{}] {}", data->messageIdNumber, data->pMessageIdName, data->pMessage);
        else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
            spdlog::warn("[{}:{}] {}", data->messageIdNumber, data->pMessageIdName, data->pMessage);
        else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
            spdlog::error("[{}:{}] {}", data->messageIdNumber, data->pMessageIdName, data->pMessage);
        else
            spdlog::critical("[{}:{}] {}", data->messageIdNumber, data->pMessageIdName, data->pMessage);

        return VK_FALSE;
    }

    void vulkan::create_messenger()
    {
        auto vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
        
        VkDebugUtilsMessengerCreateInfoEXT info = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        info.pfnUserCallback = debug_callback;

        VK_ENSURE(vkCreateDebugUtilsMessengerEXT(instance, &info, nullptr, &messenger));
    }

    void vulkan::remove_messenger()
    {
        auto vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

        vkDestroyDebugUtilsMessengerEXT(instance, messenger, nullptr);
    }
#endif
}
