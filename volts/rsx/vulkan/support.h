#include <platform.h>

#if SYS_WINDOWS
#   define VK_USE_PLATFORM_WIN32_KHR
#   define GLFW_EXPOSE_NATIVE_WIN32
#elif SYS_OSX
#   define GLFW_EXPOSE_NATIVE_COCOA
#   define VK_USE_PLATFORM_MACOS_MVK
#endif

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>

#include "volts.h"

#include <expected.h>
#include <result.h>
#include <vector>
#include <string>
#include <tuple>

#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>

#define VK_ENSURE(expr) { if(VkResult res = (expr); res != VK_SUCCESS) { spdlog::error("vk error {} = {}", #expr, res); } }

namespace volts::rsx::vulkan
{
#define STR_CASE(val) case val: return #val;
    std::string err_to_string(VkResult res)
    {
        switch(res)
        {
        STR_CASE(VK_SUCCESS);
        STR_CASE(VK_NOT_READY);
        STR_CASE(VK_TIMEOUT);
        STR_CASE(VK_EVENT_SET);
        STR_CASE(VK_EVENT_RESET);
        STR_CASE(VK_INCOMPLETE);
        STR_CASE(VK_ERROR_OUT_OF_HOST_MEMORY);
        STR_CASE(VK_ERROR_OUT_OF_DEVICE_MEMORY);
        STR_CASE(VK_ERROR_INITIALIZATION_FAILED);
        STR_CASE(VK_ERROR_DEVICE_LOST);
        STR_CASE(VK_ERROR_MEMORY_MAP_FAILED);
        STR_CASE(VK_ERROR_LAYER_NOT_PRESENT);
        STR_CASE(VK_ERROR_EXTENSION_NOT_PRESENT);
        STR_CASE(VK_ERROR_FEATURE_NOT_PRESENT);
        STR_CASE(VK_ERROR_INCOMPATIBLE_DRIVER);
        STR_CASE(VK_ERROR_TOO_MANY_OBJECTS);
        STR_CASE(VK_ERROR_FORMAT_NOT_SUPPORTED);
        STR_CASE(VK_ERROR_FRAGMENTED_POOL);
        STR_CASE(VK_ERROR_OUT_OF_POOL_MEMORY);
        STR_CASE(VK_ERROR_INVALID_EXTERNAL_HANDLE);
        STR_CASE(VK_ERROR_SURFACE_LOST_KHR);
        STR_CASE(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
        STR_CASE(VK_SUBOPTIMAL_KHR);
        STR_CASE(VK_ERROR_OUT_OF_DATE_KHR);
        STR_CASE(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
        STR_CASE(VK_ERROR_VALIDATION_FAILED_EXT);
        STR_CASE(VK_ERROR_INVALID_SHADER_NV);
        STR_CASE(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
        STR_CASE(VK_ERROR_NOT_PERMITTED_EXT);
        STR_CASE(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
        STR_CASE(VK_ERROR_FRAGMENTATION_EXT);
        STR_CASE(VK_ERROR_INVALID_DEVICE_ADDRESS_EXT);
        STR_CASE(VK_RESULT_MAX_ENUM);
        default:
            return "VK_ERROR_UNKNOWN";
        }
    }


    struct queueIndicies
    {
        svl::expected<uint32_t> graphics = svl::none();
        svl::expected<uint32_t> compute = svl::none();
        svl::expected<uint32_t> transfer = svl::none();
        svl::expected<uint32_t> present = svl::none();
    };

    svl::result<VkPipeline, VkResult> pipeline(VkDevice device,
                                               VkRenderPass pass)
    {
        return svl::err(VK_SUCCESS);
    }

    using shader = std::tuple<std::string, VkShaderStageFlagBits>;

    using shaderModules = std::vector<VkShaderModule>;

    svl::result<shaderModules, VkResult> modules(VkDevice device,
                                                 const std::vector<shader>& shaders)
    {
        // constants
        auto getLang = [](VkShaderStageFlagBits bits) {
            switch(bits)
            {
            case VK_SHADER_STAGE_VERTEX_BIT:
                return EShLangVertex;
            case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
                return EShLangTessControl;
            case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
                return EShLangTessEvaluation;
            case VK_SHADER_STAGE_GEOMETRY_BIT:
                return EShLangGeometry;
            case VK_SHADER_STAGE_FRAGMENT_BIT:
                return EShLangFragment;
            case VK_SHADER_STAGE_COMPUTE_BIT:
                return EShLangCompute;
            default:
                spdlog::critical("failed to find compile type");
                std::abort();
            }
        };

        auto rules = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

        shaderModules mods = {};

        for(uint32_t i = 0; i < shaders.size(); i++)
        {
            auto [source, bits] = shaders[i];
            auto lang = getLang(bits);

            glslang::TShader shader(lang);

            const char* strings[] = { source.c_str() };
            shader.setStrings(strings, 1);
            TBuiltInResource res = {};

            if(!shader.parse(&res, 100, false, rules))
            {
                spdlog::error("failed to parse glsl shader\n{}\n{}", 
                    shader.getInfoLog(), 
                    shader.getInfoDebugLog()
                );
                std::abort();
            }

            glslang::TProgram program;
            program.addShader(&shader);

            if(!program.link(rules))
            {
                spdlog::error("failed to link glsl program\n{}\n{}", 
                    program.getInfoLog(),
                    program.getInfoDebugLog()
                );
                std::abort();
            }

            std::vector<uint32_t> spirv;

            glslang::GlslangToSpv(*program.getIntermediate(lang), spirv);


            VkPipelineShaderStageCreateInfo stageInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
            stageInfo.stage = bits;
            stageInfo.pName = "main";

            VkShaderModuleCreateInfo moduleInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
            moduleInfo.codeSize = spirv.size() * sizeof(uint32_t);
            moduleInfo.pCode = spirv.data();

            VkShaderModule mod;
            if(VkResult res = vkCreateShaderModule(device, &moduleInfo, nullptr, &mod); res < 0)
                return svl::err(res);

            mods.push_back(mod);
        }

        return svl::ok(mods);
    }

    using depthstencil = std::tuple<VkImage, VkDeviceMemory, VkImageView>;

    svl::result<depthstencil, VkResult> depthStencil(VkDevice device, 
                                                     VkPhysicalDevice physical,
                                                     VkFormat format, 
                                                     VkExtent2D extent)
    {
        // image

        VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = format;
        imageInfo.extent = { extent.width, extent.height, 1 };
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = NUM_SAMPLES;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physical, VK_FORMAT_D16_UNORM, &props);

        if(props.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
            imageInfo.tiling = VK_IMAGE_TILING_LINEAR;
        else if(props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        else
            V_ASSERT(false);

        VkImage image;
        if(VkResult res = vkCreateImage(device, &imageInfo, nullptr, &image); res < 0)
            return svl::err(res);

        // memory allocate

        VkMemoryRequirements require;
        vkGetImageMemoryRequirements(device, image, &require);

        VkPhysicalDeviceMemoryProperties memoryProps;
        vkGetPhysicalDeviceMemoryProperties(physical, &memoryProps);

        VkMemoryAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
        allocInfo.allocationSize = require.size;

        uint32_t bits = require.memoryTypeBits;

        for(uint32_t i = 0; i < memoryProps.memoryTypeCount; i++)
        {
            if(bits & 1 && memoryProps.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
            {
                allocInfo.memoryTypeIndex = i;
                break;
            }

            bits >>= 1;
        }

        VkDeviceMemory memory;

        if(VkResult res = vkAllocateMemory(device, &allocInfo, nullptr, &memory); res < 0)
            return svl::err(res);

        if(VkResult res = vkBindImageMemory(device, image, memory, 0); res < 0)
            return svl::err(res);

        // image view
        
        VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        viewInfo.format = format;
        viewInfo.image = image;
        viewInfo.components = {
            VK_COMPONENT_SWIZZLE_R,
            VK_COMPONENT_SWIZZLE_G,
            VK_COMPONENT_SWIZZLE_B,
            VK_COMPONENT_SWIZZLE_A
        };

        viewInfo.subresourceRange = {
            VK_IMAGE_ASPECT_DEPTH_BIT,
            0, 1,
            0, 1
        };

        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

        VkImageView view;
        if(VkResult res = vkCreateImageView(device, &viewInfo, nullptr, &view); res < 0)
            return svl::err(res);

        return svl::ok(std::make_tuple(image, memory, view));
    }

    svl::result<std::vector<VkFramebuffer>, VkResult> framebuffers(VkDevice device,
                                                                   VkExtent2D size,
                                                                   VkRenderPass pass,
                                                                   const std::vector<VkImageView>& views)
    {
        std::vector<VkFramebuffer> buffers(views.size());

        for(uint32_t i = 0; i < views.size(); i++)
        {
            VkFramebufferCreateInfo createInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
            createInfo.renderPass = pass;
            createInfo.attachmentCount = 1;
            createInfo.pAttachments = &views[i];
            createInfo.width = size.width;
            createInfo.height = size.height;
            createInfo.layers = 1;

            if(VkResult res = vkCreateFramebuffer(device, &createInfo, nullptr, &buffers[i]); res < 0)
                return svl::err(res);
        }

        return svl::ok(buffers);
    }

    svl::result<VkRenderPass, VkResult> renderpass(VkDevice device, 
                                                   VkFormat format)
    {
        VkAttachmentDescription attach = {};
        attach.format = format;
        attach.samples = VK_SAMPLE_COUNT_1_BIT;
        attach.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attach.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attach.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attach.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attach.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        attach.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference ref = {};
        ref.attachment = 0;
        ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription desc = {};
        desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        desc.colorAttachmentCount = 1;
        desc.pColorAttachments = &ref;

        VkRenderPassCreateInfo createInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = &attach;
        createInfo.subpassCount = 1;
        createInfo.pSubpasses = &desc;

        VkRenderPass out;

        if(VkResult res = vkCreateRenderPass(device, &createInfo, nullptr, &out); res < 0)
            return svl::err(res);

        return svl::ok(out);
    }

    svl::result<std::vector<VkImageView>, VkResult> imageViews(VkDevice device,
                                                               VkFormat format,
                                                               const std::vector<VkImage>& images)
    {
        std::vector<VkImageView> views(images.size());

        for(uint32_t i = 0; i < images.size(); i++)
        {
            VkImageViewCreateInfo createInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
            createInfo.image = images[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = format;
            
            createInfo.components = { 
                VK_COMPONENT_SWIZZLE_IDENTITY, 
                VK_COMPONENT_SWIZZLE_IDENTITY, 
                VK_COMPONENT_SWIZZLE_IDENTITY, 
                VK_COMPONENT_SWIZZLE_IDENTITY 
            };

            createInfo.subresourceRange = {
                VK_IMAGE_ASPECT_COLOR_BIT,
                0, 1,
                0, 1
            };

            if(VkResult res = vkCreateImageView(device, &createInfo, nullptr, &views[i]); res < 0)
                return svl::err(res);
        }

        return svl::ok(views);
    }

    svl::result<std::vector<VkImage>, VkResult> images(VkDevice device, VkSwapchainKHR swap)
    {
        uint32_t num = 0;
        if(VkResult res = vkGetSwapchainImagesKHR(device, swap, &num, nullptr); res < 0)
            return svl::err(res);
        
        V_ASSERT(num > 0);

        std::vector<VkImage> images(num);
        if(VkResult res = vkGetSwapchainImagesKHR(device, swap, &num, images.data()); res < 0)
            return svl::err(res);

        return svl::ok(images);
    }

    using swap = std::tuple<VkSwapchainKHR, VkFormat, VkExtent2D>;

    svl::result<swap, VkResult> swapchain(VkDevice device,
                                          VkPhysicalDevice physical,
                                          VkSurfaceKHR surface,
                                          glm::u32vec2 size,
                                          queueIndicies queues,
                                          VkSwapchainKHR old = VK_NULL_HANDLE)
    {
        VkSurfaceCapabilitiesKHR caps;
        if(VkResult res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical, surface, &caps); res < 0)
            return svl::err(res);

        // get formats

        uint32_t numFormats = 0;
        if(VkResult res = vkGetPhysicalDeviceSurfaceFormatsKHR(physical, surface, &numFormats, nullptr); res < 0)
            return svl::err(res);

        V_ASSERT(numFormats != 0);

        auto* formats = (VkSurfaceFormatKHR*)salloc(sizeof(VkSurfaceFormatKHR) * numFormats);
        if(VkResult res = vkGetPhysicalDeviceSurfaceFormatsKHR(physical, surface, &numFormats, formats); res < 0)
            return svl::err(res);

        /// get present modes

        uint32_t numModes = 0;
        if(VkResult res = vkGetPhysicalDeviceSurfacePresentModesKHR(physical, surface, &numModes, nullptr); res < 0)
            return svl::err(res);

        auto* modes = (VkPresentModeKHR*)salloc(sizeof(VkPresentModeKHR) * numModes);
        if(VkResult res = vkGetPhysicalDeviceSurfacePresentModesKHR(physical, surface, &numModes, modes); res < 0)
            return svl::err(res);

        
        // get swapchain size

        auto extent = caps.currentExtent.width == std::numeric_limits<uint32_t>::max() ? VkExtent2D {
            std::clamp(size.x, caps.minImageExtent.width, caps.maxImageExtent.width),
            std::clamp(size.y, caps.minImageExtent.height, caps.maxImageExtent.height)
        } : caps.currentExtent;

        // if we have it use mailbox otherwise use fifo
        auto mode = std::find(modes, modes + numModes, VK_PRESENT_MODE_MAILBOX_KHR) != modes + numModes
            ? VK_PRESENT_MODE_MAILBOX_KHR 
            : VK_PRESENT_MODE_FIFO_KHR;

        // todo: search for better formats before fallback
        auto format = numFormats == 1 && formats[0].format == VK_FORMAT_UNDEFINED
            ? VkSurfaceFormatKHR { VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR } 
            : formats[0];

        // get hte transform type, favoring VK_SURFACE_TRANSFORM_IDENTITY_BIT
        auto transform = caps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
            ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
            : caps.currentTransform;

        // get the minimum amount of images needed
        auto count = std::min<uint32_t>(
            caps.maxImageCount ? caps.maxImageCount : 1,
            caps.minImageCount + 1
        );

        // find the best supported composite alpha flag
        VkCompositeAlphaFlagBitsKHR alphaFlag = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        for(VkCompositeAlphaFlagBitsKHR flag : {
            VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR
        })
        {
            if(caps.supportedCompositeAlpha & flag)
            {
                alphaFlag = flag;
                break;
            }
        }

        VkSwapchainCreateInfoKHR createInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        createInfo.surface = surface;
        createInfo.minImageCount = count;
        createInfo.imageFormat = format.format;
        createInfo.imageColorSpace = format.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        createInfo.preTransform = transform;
        createInfo.compositeAlpha = alphaFlag;
        createInfo.presentMode = mode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = old;

        // if the present and graphics queues are seperate we need to account for that
        if(queues.graphics.value() == queues.present.value())
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }
        else
        {
            const uint32_t indicies[] = {
                queues.graphics.value(),
                queues.present.value()
            };

            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = indicies;
        }

        VkSwapchainKHR out;
        if(VkResult res = vkCreateSwapchainKHR(device, &createInfo, nullptr, &out); res < 0)
            return svl::err(res);

        return svl::ok(std::make_tuple(out, format.format, extent));
    }

    svl::result<VkSemaphore, VkResult> semaphore(VkDevice device) 
    {
        VkSemaphoreCreateInfo createInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
        
        VkSemaphore out;
        if(VkResult res = vkCreateSemaphore(device, &createInfo, nullptr, &out); res < 0)
            return svl::err(res);

        return svl::ok(out);
    }

    svl::result<VkSurfaceKHR, VkResult> surface(VkInstance instance, GLFWwindow* window)
    {
        VkSurfaceKHR surface;
        
        if(VkResult res = glfwCreateWindowSurface(instance, window, nullptr, &surface); res < 0)
            return svl::err(res);

        return svl::ok(surface);
    }

    svl::expected<queueIndicies> queues(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        uint32_t num = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &num, nullptr);
        
        if(!num)
            return svl::none();

        std::vector<VkQueueFamilyProperties> props(num);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &num, props.data());

        if(!num)
            return svl::none();


        VkBool32* canPresent = (VkBool32*)salloc(sizeof(VkBool32) * num);
        for(uint32_t i = 0; i < num; i++)
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &canPresent[i]);


        queueIndicies indicies = {};

        for(uint32_t i = 0; i < props.size(); i++)
        {
            auto flags = props[i].queueFlags;

            if(static bool once = true; once && flags & VK_QUEUE_GRAPHICS_BIT)
            {
                once = false;
                indicies.graphics = i;
            }

            if(static bool once = true; once && flags & VK_QUEUE_COMPUTE_BIT)
            {
                once = false;
                indicies.compute = i;
            }

            if(static bool once = true; once && flags & VK_QUEUE_TRANSFER_BIT)
            {
                once = false;
                indicies.transfer = i;
            }

            if(static bool once = true; once && canPresent[i] == VK_TRUE)
            {
                once = false;
                indicies.present = i;
            }
        }

        return indicies;
    }

    svl::result<VkDevice, VkResult> device(VkPhysicalDevice physical, 
                                           uint32_t queueIndex,
                                           const std::vector<std::string>& extensions = {})
    {
        float prioritys[] = { 1.f };

        VkDeviceQueueCreateInfo queueInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
        queueInfo.queueCount = 1;
        queueInfo.pQueuePriorities = prioritys;
        queueInfo.queueFamilyIndex = queueIndex;

        VkDeviceCreateInfo deviceInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
        deviceInfo.queueCreateInfoCount = 1;
        deviceInfo.pQueueCreateInfos = &queueInfo;
        
        const char** extNames = (const char**)salloc(sizeof(char*) * extensions.size());
        for(uint32_t i = 0; i < extensions.size(); i++)
            extNames[i] = extensions[i].c_str();

        deviceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        deviceInfo.ppEnabledExtensionNames = extNames;

        VkDevice out;

        if(VkResult res = vkCreateDevice(physical, &deviceInfo, nullptr, &out); res < 0)
            return svl::err(res);

        return svl::ok(out);
    }

    using commandData = std::tuple<VkCommandPool, VkCommandBuffer>;

    svl::result<commandData, VkResult> commandPool(VkDevice device, uint32_t index)
    {
        VkCommandPoolCreateInfo createInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        createInfo.queueFamilyIndex = index;

        VkCommandPool pool;

        if(VkResult res = vkCreateCommandPool(device, &createInfo, nullptr, &pool); res < 0)
            return svl::err(res);


        VkCommandBufferAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        allocInfo.commandPool = pool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer buffer;

        if(VkResult res = vkAllocateCommandBuffers(device, &allocInfo, &buffer); res < 0)
            return svl::err(res);

        return svl::ok(std::make_tuple(pool, buffer));
    }

    svl::result<std::vector<VkPhysicalDevice>, VkResult> physicalDevices(VkInstance instance)
    {
        uint32_t num = 0;

        if(VkResult res = vkEnumeratePhysicalDevices(instance, &num, nullptr); res < 0)
            return svl::err(res);

        std::vector<VkPhysicalDevice> devices(num);
        if(VkResult res = vkEnumeratePhysicalDevices(instance, &num, devices.data()); res < 0)
            return svl::err(res);

        return svl::ok(devices);
    }

    svl::result<std::vector<VkExtensionProperties>, VkResult> extensions(const svl::expected<std::string>& layer = svl::none())
    {
        const char* name = layer.valid() ? layer.value().c_str() : nullptr;

        uint32_t num = 0;
        
        if(VkResult res = vkEnumerateInstanceExtensionProperties(name, &num, nullptr); res < 0)
            return svl::err(res);

        std::vector<VkExtensionProperties> props(num);

        if(VkResult res = vkEnumerateInstanceExtensionProperties(name, &num, props.data()); res < 0)
            return svl::err(res);

        return svl::ok(props);
    }

    svl::result<std::vector<VkExtensionProperties>, VkResult> extensions(VkPhysicalDevice device, 
                                                                         const svl::expected<std::string>& layer = svl::none())
    {
        const char* name = layer.valid() ? layer.value().c_str() : nullptr;
        uint32_t num = 0;

        if(VkResult res = vkEnumerateDeviceExtensionProperties(device, name, &num, nullptr); res < 0)
            return svl::err(res);

        std::vector<VkExtensionProperties> props(num);
        if(VkResult res = vkEnumerateDeviceExtensionProperties(device, name, &num, props.data()); res < 0)
            return svl::err(res);

        return svl::ok(props);
    }

    svl::result<std::vector<VkLayerProperties>, VkResult> layers(VkPhysicalDevice device)
    {
        uint32_t num = 0;
        if(VkResult res = vkEnumerateDeviceLayerProperties(device, &num, nullptr); res < 0)
            return svl::err(res);

        std::vector<VkLayerProperties> props(num);

        if(VkResult res = vkEnumerateDeviceLayerProperties(device, &num, props.data()); res < 0)
            return svl::err(res);

        return svl::ok(props);
    }

    svl::result<std::vector<VkLayerProperties>, VkResult> layers()
    {
        uint32_t num = 0;
        if(VkResult res = vkEnumerateInstanceLayerProperties(&num, nullptr); res < 0)
            return svl::err(res);

        std::vector<VkLayerProperties> props(num);

        if(VkResult res = vkEnumerateInstanceLayerProperties(&num, props.data()); res < 0)
            return svl::err(res);

        return svl::ok(props);
    }

    svl::result<VkInstance, VkResult> instance(const std::string& name,
                                               const std::vector<std::string>& extensions = {},
                                               const std::vector<std::string>& layers = {})
    {
        // create application info
        VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
        
        // for now we only use the minimum version
        appInfo.apiVersion = VK_API_VERSION_1_0;
        
        // set the application name and version
        appInfo.pApplicationName = name.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

        // set the engine name and version
        appInfo.pEngineName = "emulated rsx";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);


        // setup the create info struct
        VkInstanceCreateInfo createInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };

        createInfo.pApplicationInfo = &appInfo;


        // create char** arrays of layer and extension names
        const char** layerNames = (const char**)salloc(sizeof(char*) * layers.size());
        const char** extensionNames = (const char**)salloc(sizeof(char*) * extensions.size());

        for(int i = 0; i < extensions.size(); i++)
            extensionNames[i] = extensions[i].c_str();

        for(int i = 0; i < layers.size(); i++)
            layerNames[i] = layers[i].c_str();

        createInfo.ppEnabledExtensionNames = extensionNames;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());

        createInfo.ppEnabledLayerNames = layerNames;
        createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());

        // create the instance
        VkInstance out;
        if(VkResult res = vkCreateInstance(&createInfo, nullptr, &out); res < 0)
            return svl::err(res);

        return svl::ok(out);
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
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

    svl::result<VkDebugUtilsMessengerEXT, VkResult> addDebugger(VkInstance instance, 
                                                                PFN_vkDebugUtilsMessengerCallbackEXT callback = debugCallback)
    {
        auto vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
        
        VkDebugUtilsMessengerCreateInfoEXT messenger = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        messenger.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        messenger.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        messenger.pfnUserCallback = callback;


        VkDebugUtilsMessengerEXT out;
        if(VkResult res = vkCreateDebugUtilsMessengerEXT(instance, &messenger, nullptr, &out); res < 0)
            return svl::err(res);

        return svl::ok(out);
    }

    void removeDebugger(VkInstance instance, VkDebugUtilsMessengerEXT messenger)
    {
        auto vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

        vkDestroyDebugUtilsMessengerEXT(instance, messenger, nullptr);
    }
}