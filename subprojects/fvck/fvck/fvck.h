#pragma once

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <vector>
#include <string>
#include <string>
#include <optional>
#include <set>
#include <algorithm>

#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>

#include <spdlog/spdlog.h>

#ifndef FVCK_MIN_VERSION 
#   define FVCK_MIN_VERSION VK_API_VERSION_1_0
#endif

#define FVCK_ENSURE(...) { if(VkResult res = (__VA_ARGS__); res) { spdlog::error("[{}:{}] = {}", __FILE__, __LINE__, fvck::to_string(res)); } }

namespace fvck
{

#define FVCK_CASE(val) case val: return #val;
    
    const char* to_string(VkResult res)
    {
        switch(res)
        {
        FVCK_CASE(VK_SUCCESS);
        FVCK_CASE(VK_NOT_READY);
        FVCK_CASE(VK_TIMEOUT);
        FVCK_CASE(VK_EVENT_SET);
        FVCK_CASE(VK_EVENT_RESET);
        FVCK_CASE(VK_INCOMPLETE);
        FVCK_CASE(VK_ERROR_OUT_OF_HOST_MEMORY);
        FVCK_CASE(VK_ERROR_OUT_OF_DEVICE_MEMORY);
        FVCK_CASE(VK_ERROR_INITIALIZATION_FAILED);
        FVCK_CASE(VK_ERROR_DEVICE_LOST);
        FVCK_CASE(VK_ERROR_MEMORY_MAP_FAILED);
        FVCK_CASE(VK_ERROR_LAYER_NOT_PRESENT);
        FVCK_CASE(VK_ERROR_EXTENSION_NOT_PRESENT);
        FVCK_CASE(VK_ERROR_FEATURE_NOT_PRESENT);
        FVCK_CASE(VK_ERROR_INCOMPATIBLE_DRIVER);
        FVCK_CASE(VK_ERROR_TOO_MANY_OBJECTS);
        FVCK_CASE(VK_ERROR_FORMAT_NOT_SUPPORTED);
        FVCK_CASE(VK_ERROR_FRAGMENTED_POOL);
        FVCK_CASE(VK_ERROR_OUT_OF_POOL_MEMORY);
        FVCK_CASE(VK_ERROR_INVALID_EXTERNAL_HANDLE);
        FVCK_CASE(VK_ERROR_SURFACE_LOST_KHR);
        FVCK_CASE(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
        FVCK_CASE(VK_SUBOPTIMAL_KHR);
        FVCK_CASE(VK_ERROR_OUT_OF_DATE_KHR);
        FVCK_CASE(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
        FVCK_CASE(VK_ERROR_VALIDATION_FAILED_EXT);
        FVCK_CASE(VK_ERROR_INVALID_SHADER_NV);
        FVCK_CASE(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
        FVCK_CASE(VK_ERROR_NOT_PERMITTED_EXT);
        FVCK_CASE(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
        FVCK_CASE(VK_ERROR_FRAGMENTATION_EXT);
        FVCK_CASE(VK_ERROR_INVALID_DEVICE_ADDRESS_EXT);
        FVCK_CASE(VK_RESULT_MAX_ENUM);
        default:
            return "VK_ERROR_UNKNOWN";
        }
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL debugMessengerCallback(
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

#undef FVCK_CAST

    struct QueueIndicies
    {
        uint32_t graphics = UINT32_MAX;
        uint32_t compute = UINT32_MAX;
        uint32_t present = UINT32_MAX;
        uint32_t transfer = UINT32_MAX;
    };

    struct ShaderModule
    {
        VkShaderModule shader;
    };

    struct Surface
    {
        Surface() : surface(VK_NULL_HANDLE) {}

        Surface(VkSurfaceKHR surf, GLFWwindow* parent)
            : surface(surf)
            , window(parent)
        {}

        GLFWwindow* window;
        VkSurfaceKHR surface;
    };

    struct Queue
    {
        Queue() : queue(VK_NULL_HANDLE) {}
        Queue(VkQueue q)
            : queue(q)
        {}

        VkQueue queue;
    };

    struct SwapchainImage
    {
        SwapchainImage(VkImage img, VkImageView v)
            : image(img)
            , view(v)
        {}

        VkImage image;
        VkImageView view;
    };

    struct Swapchain
    {
        Swapchain() : swapchain(VK_NULL_HANDLE) {}
        
        Swapchain(VkSwapchainKHR swap, VkDevice parent, VkFormat f)
            : swapchain(swap)
            , device(parent)
            , format(f)
        {}

        std::vector<SwapchainImage> images() const
        {
            uint32_t num = 0;
            vkGetSwapchainImagesKHR(device, swapchain, &num, nullptr);

            std::vector<VkImage> imgs(num);
            vkGetSwapchainImagesKHR(device, swapchain, &num, imgs.data());

            std::vector<SwapchainImage> out;
            for(auto img : imgs)
            {
                VkImageViewCreateInfo createInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
                createInfo.image = img;
                createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                createInfo.format = format;

                createInfo.components = {
                    VK_COMPONENT_SWIZZLE_IDENTITY,
                    VK_COMPONENT_SWIZZLE_IDENTITY,
                    VK_COMPONENT_SWIZZLE_IDENTITY,
                    VK_COMPONENT_SWIZZLE_IDENTITY
                };

                createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                createInfo.subresourceRange.baseMipLevel = 0;
                createInfo.subresourceRange.levelCount = 1;
                createInfo.subresourceRange.levelCount = 1;
                createInfo.subresourceRange.baseArrayLayer = 0;
                createInfo.subresourceRange.layerCount = 1;

                VkImageView view;

                FVCK_ENSURE(vkCreateImageView(device, &createInfo, nullptr, &view));

                out.push_back({ img, view });
            }

            return out;
        }

        VkSwapchainKHR swapchain;
        VkDevice device;
        VkFormat format;
    };

    struct ShaderData
    {
        const char* source;
        const char* name;
        VkShaderStageFlagBits stage;
    };

    struct Device
    {
        Device() : device(VK_NULL_HANDLE) {}
        Device(VkDevice d, QueueIndicies queues, VkPhysicalDevice parent)
            : device(d)
            , indicies(queues)
            , physical(parent)
        {}

        ShaderModule compile(const ShaderData& shaderData)
        {
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

            auto resources = [] {
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
            };

            auto rules = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

            auto& [source, name, bits] = shaderData;
                
            spdlog::debug("compiling shader {}", name);
            auto lang = getLang(bits);

            glslang::TShader shader(lang);

            auto res = resources();

            const char* strings[] = { source };
            shader.setStrings(strings, 1);

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
            FVCK_ENSURE(vkCreateShaderModule(device, &moduleInfo, nullptr, &mod));
            
            return {mod};
        }

        Swapchain swapchain(const Surface& surface) const
        {
            VkSurfaceCapabilitiesKHR caps;
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical, surface.surface, &caps);


            uint32_t numFormats = 0;
            vkGetPhysicalDeviceSurfaceFormatsKHR(physical, surface.surface, &numFormats, nullptr);

            std::vector<VkSurfaceFormatKHR> formats(numFormats);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physical, surface.surface, &numFormats, formats.data());
        
            
            uint32_t numModes = 0;
            vkGetPhysicalDeviceSurfacePresentModesKHR(physical, surface.surface, &numModes, nullptr);

            std::vector<VkPresentModeKHR> modes(numModes);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physical, surface.surface, &numModes, modes.data());
        
        
            if(formats.empty() || modes.empty())
            {
                spdlog::critical("swapchain modes or formats are empty");
                std::abort();
            }

            VkSurfaceFormatKHR format = [&formats] {
                for(auto format : formats)
                    if(format.format == VK_FORMAT_R8G8B8A8_UNORM)
                        return format;

                return formats[0];
            }();

            VkPresentModeKHR mode = [&modes] {
                for(auto mode : modes)
                    if(mode == VK_PRESENT_MODE_MAILBOX_KHR)
                        return mode;

                return VK_PRESENT_MODE_FIFO_KHR;
            }();

            VkExtent2D extent = [&caps, &surface] {
                if(caps.currentExtent.width != UINT32_MAX)
                    return caps.currentExtent;

                int w, h;
                glfwGetFramebufferSize(surface.window, &w, &h);

                VkExtent2D extent;

                extent.width = std::clamp<uint32_t>(w, caps.minImageExtent.width, caps.maxImageExtent.width);
                extent.height = std::clamp<uint32_t>(h, caps.minImageExtent.height, caps.maxImageExtent.height);
            
                return extent;
            }();
        
            uint32_t imageCount = std::min<uint32_t>(
                caps.maxImageCount ? caps.maxImageCount : 1,
                caps.minImageCount + 1
            );

            auto transform = caps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
                ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
                : caps.currentTransform;


            VkSwapchainCreateInfoKHR createInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
            createInfo.imageFormat = format.format;
            createInfo.imageColorSpace = format.colorSpace;
            createInfo.imageExtent = extent;
            createInfo.imageArrayLayers = 1;
            createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            uint32_t families[] = { indicies.graphics, indicies.present };

            if(indicies.graphics != indicies.present)
            {
                createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                createInfo.queueFamilyIndexCount = 2;
                createInfo.pQueueFamilyIndices = families;
            }
            else
            {
                createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            }

            createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

            createInfo.presentMode = mode;
            createInfo.clipped = VK_TRUE;
            createInfo.surface = surface.surface;
            createInfo.preTransform = transform;
            createInfo.minImageCount = imageCount;

            // TODO: we may need to provide a new swapchain
            createInfo.oldSwapchain = VK_NULL_HANDLE;

            VkSwapchainKHR out;

            FVCK_ENSURE(vkCreateSwapchainKHR(device, &createInfo, nullptr, &out));

            return { out, device, format.format };
        }

        Queue presentQueue() const
        {
            VkQueue out;
            vkGetDeviceQueue(device, indicies.present, 0, &out);

            return out;
        }

        Queue graphicsQueue() const
        {
            VkQueue out;
            vkGetDeviceQueue(device, indicies.graphics, 0, &out);

            return out;
        }

        Queue computeQueue() const
        {
            VkQueue out;
            vkGetDeviceQueue(device, indicies.compute, 0, &out);

            return out;
        }

        Queue transferQueue() const
        {
            VkQueue out;
            vkGetDeviceQueue(device, indicies.transfer, 0, &out);

            return out;
        }

        QueueIndicies indicies;
        VkDevice device;
        VkPhysicalDevice physical;
    };

    struct PhysicalDevice
    {
        PhysicalDevice() : physical(VK_NULL_HANDLE) {}
        PhysicalDevice(VkPhysicalDevice device)
            : physical(device)
        {
            vkGetPhysicalDeviceProperties(physical, &props);
            vkGetPhysicalDeviceFeatures(physical, &features);

            {
                uint32_t num = 0;
                vkGetPhysicalDeviceQueueFamilyProperties(physical, &num, nullptr);

                queues.resize(num);
                vkGetPhysicalDeviceQueueFamilyProperties(physical, &num, queues.data());
            }
        }

        uint32_t score() const
        {
            // we require geometry shaders
            if(!features.geometryShader)
                return 0;

            uint32_t ret = 0;
            if(props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                ret += 1000;

            ret += props.limits.maxImageDimension2D;

            return ret;
        }

        QueueIndicies indicies() const
        {
            QueueIndicies out = {};

            for(uint32_t i = 0; i < queues.size(); i++)
            {
                if(out.graphics == UINT32_MAX && queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                    out.graphics = i;

                if(out.compute == UINT32_MAX && queues[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
                    out.compute = i;

                if(out.transfer == UINT32_MAX && queues[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
                    out.transfer = i;
            }

            return out;
        }

        QueueIndicies indicies(const Surface& surface) const
        {
            QueueIndicies out = {};

            for(uint32_t i = 0; i < queues.size(); i++)
            {
                if(out.graphics == UINT32_MAX && queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                    out.graphics = i;

                if(out.compute == UINT32_MAX && queues[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
                    out.compute = i;

                if(out.transfer == UINT32_MAX && queues[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
                    out.transfer = i;

                if(out.present == UINT32_MAX)
                {
                    VkBool32 supportsPresent = VK_FALSE;
                    vkGetPhysicalDeviceSurfaceSupportKHR(physical, i, surface.surface, &supportsPresent);

                    if(supportsPresent)
                        out.present = i;
                }
            }

            return out;
        }

        Device device(const Surface& surface,
                      const std::vector<const char*>& extensions = {}) const
        {
            QueueIndicies queues = indicies(surface);

            if((queues.present | queues.graphics) == UINT32_MAX)
            {
                spdlog::critical("lacking present or graphics support");
                std::abort();
            }

            float priority = 1.f;

            std::vector<VkDeviceQueueCreateInfo> queueInfos = {};

            // we use a set to make sure we dont create multiple queues with the same family index
            // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/vkspec.html#VUID-VkDeviceCreateInfo-queueFamilyIndex-00372
            for(auto queueIndex : std::set<uint32_t>({ queues.present, queues.graphics }))
            {
                VkDeviceQueueCreateInfo queueInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
                queueInfo.queueFamilyIndex = queueIndex;
                queueInfo.queueCount = 1;
                queueInfo.pQueuePriorities = &priority;
                
                queueInfos.push_back(queueInfo);
            }

            VkDeviceCreateInfo createInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };

            createInfo.pQueueCreateInfos = queueInfos.data();
            createInfo.queueCreateInfoCount = queueInfos.size();

            VkPhysicalDeviceFeatures requiredFeatures = {};
            createInfo.pEnabledFeatures = &requiredFeatures;

            createInfo.ppEnabledExtensionNames = extensions.data();
            createInfo.enabledExtensionCount = extensions.size();

            VkDevice out;

            FVCK_ENSURE(vkCreateDevice(physical, &createInfo, nullptr, &out));

            return { out, queues, physical };
        }
        
        const VkPhysicalDeviceProperties& properties() const { return props; }
        const VkPhysicalDeviceFeatures& feats() const { return features; }

        VkPhysicalDevice physical;
        VkPhysicalDeviceProperties props;
        VkPhysicalDeviceFeatures features;
        std::vector<VkQueueFamilyProperties> queues;
    };

    struct Instance
    {
        Instance() : instance(VK_NULL_HANDLE) {}
        Instance(
            const std::string& appName,
            std::vector<const char*> extensions = {},
            std::vector<const char*> layers = {},
            uint32_t appVersion = VK_MAKE_VERSION(1, 0, 0)
        )
        {
            // add the glfw required extensions
            {
                uint32_t num = 0;
                const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&num);

                for(uint32_t i = 0; i < num; i++)
                    extensions.push_back(glfwExtensions[i]);
            }
            
            VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };

            appInfo.pApplicationName = appName.c_str();
            appInfo.applicationVersion = appVersion;

            appInfo.pEngineName = "fvck";
            appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

            VkInstanceCreateInfo createInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
            createInfo.pApplicationInfo = &appInfo;

            createInfo.ppEnabledExtensionNames = extensions.data();
            createInfo.enabledExtensionCount = extensions.size();

            createInfo.ppEnabledLayerNames = layers.data();
            createInfo.enabledLayerCount = layers.size();

            FVCK_ENSURE(vkCreateInstance(&createInfo, nullptr, &instance));
        }

        void destroy()
        {
            vkDestroyInstance(instance, nullptr);
        }

        std::vector<PhysicalDevice> devices() const
        {
            uint32_t num = 0;
            FVCK_ENSURE(vkEnumeratePhysicalDevices(instance, &num, nullptr));

            std::vector<VkPhysicalDevice> physicals(num);
            FVCK_ENSURE(vkEnumeratePhysicalDevices(instance, &num, physicals.data()));

            std::vector<PhysicalDevice> out;
            for(auto physical : physicals)
                out.push_back(physical);

            return out;
        }

        VkDebugUtilsMessengerEXT debugMessenger(PFN_vkDebugUtilsMessengerCallbackEXT callback = debugMessengerCallback)
        {
            VkDebugUtilsMessengerCreateInfoEXT createInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };

            createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
            createInfo.pfnUserCallback = callback;

            VkDebugUtilsMessengerEXT out;

            FVCK_ENSURE(vkCreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &out));

            return out;
        }

        void removeMessenger(VkDebugUtilsMessengerEXT messenger)
        {
            auto vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
            vkDestroyDebugUtilsMessengerEXT(instance, messenger, nullptr);
        }

        Surface surface(GLFWwindow* window) const
        {
            VkSurfaceKHR surf;
            FVCK_ENSURE(glfwCreateWindowSurface(instance, window, nullptr, &surf));

            return { surf, window };
        }

        VkInstance instance;
    };

    std::vector<VkLayerProperties> layers()
    {
        uint32_t num = 0;
        vkEnumerateInstanceLayerProperties(&num, nullptr);

        std::vector<VkLayerProperties> props(num);
        vkEnumerateInstanceLayerProperties(&num, props.data());

        return props;
    }

    std::vector<VkExtensionProperties> extensions(const std::string& layer = "")
    {
        const char* layerName = layer.empty() ? nullptr : layer.c_str();
        
        uint32_t num = 0;
        vkEnumerateInstanceExtensionProperties(layerName, &num, nullptr);

        std::vector<VkExtensionProperties> props(num);
        vkEnumerateInstanceExtensionProperties(layerName, &num, props.data());
        
        return props;
    }
}
