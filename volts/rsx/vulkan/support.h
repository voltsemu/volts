#include <vulkan/vulkan.h>

#define VK_ENSURE(expr) { if(VkResult res = (expr); res != VK_SUCCESS) { spdlog::error("vk error {} = {}", #expr, res); } }

#include "volts.h"

#include <platform.h>

#if SYS_WINDOWS
#   include <Windows.h>
#endif

#include <GLFW/glfw3.h>

#include <optional>
#include <vector>
#include <string>

#include <spdlog/spdlog.h>

namespace volts::rsx
{
    struct physical_device : device
    {
        physical_device(VkPhysicalDevice phys)
            : physical(phys)
        {
            // get basic information
            vkGetPhysicalDeviceProperties(physical, &props);
            vkGetPhysicalDeviceFeatures(physical, &features);
            vkGetPhysicalDeviceMemoryProperties(physical, &memprops);

            // get extensions
            {
                // get number of extensions
                uint32_t num = 0;
                VK_ENSURE(vkEnumerateDeviceExtensionProperties(physical, nullptr, &num, nullptr));
                
                // get the actual extensions
                extensions.reserve(num);
                VK_ENSURE(vkEnumerateDeviceExtensionProperties(physical, nullptr, &num, extensions.data()));
            }

            for(auto ext : extensions)
                spdlog::info("device extension: {}", ext.extensionName);

            // get queue families
            {
                // get number of family properties
                uint32_t num = 0;
                vkGetPhysicalDeviceQueueFamilyProperties(physical, &num, nullptr);

                // get the families
                families.resize(num);
                vkGetPhysicalDeviceQueueFamilyProperties(physical, &num, families.data());
            }
        }

        virtual ~physical_device() override {}

        virtual std::string name() const override
        {
            return props.deviceName;
        }

        template<VkQueueFlags T>
        VkDevice device(bool swapchain = false) const
        {
            static_assert(T & (VK_QUEUE_TRANSFER_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_GRAPHICS_BIT), "invalid device type");
            
            const float priority = 0.f;

            std::vector<VkDeviceQueueCreateInfo> queues;

            for(VkQueueFlagBits flag : { VK_QUEUE_TRANSFER_BIT, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT })
            {
                // check if we need this queue
                if(!(T & flag))
                    continue;

                VkDeviceQueueCreateInfo createinfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
                
                createinfo.queueFamilyIndex = queue_index(flag).value();
                createinfo.queueCount = 1;
                createinfo.pQueuePriorities = &priority;

                queues.push_back(createinfo);
            }

            std::vector<const char*> exts;

            if(swapchain)
            {
                V_ASSERT(supports(VK_KHR_SWAPCHAIN_EXTENSION_NAME));
                exts.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
            }

            VkDeviceCreateInfo device_create_info = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
            device_create_info.queueCreateInfoCount = (uint32_t)queues.size();
            device_create_info.pQueueCreateInfos = queues.data();

            // TODO: probably isnt needed
            // device_create_info.pEnabledFeatures

#if VK_VALIDATION
            if(supports(VK_EXT_DEBUG_MARKER_EXTENSION_NAME))
            {
                exts.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
                spdlog::info("added debug marker extension");
            }
#endif

            device_create_info.enabledExtensionCount = (uint32_t)exts.size();
            device_create_info.ppEnabledExtensionNames = exts.data();
            
            VkDevice logical;

            VK_ENSURE(vkCreateDevice(physical, &device_create_info, nullptr, &logical));

            return logical;
        }

        bool supports(const std::string& extension) const
        {
            for(auto ext : extensions)
                if(extension == ext.extensionName)
                    return true;

            return false;
        }

        std::optional<VkFormat> best_format() const
        {
            for(VkFormat format : {
                VK_FORMAT_D32_SFLOAT_S8_UINT, 
                VK_FORMAT_D32_SFLOAT, 
                VK_FORMAT_D24_UNORM_S8_UINT, 
                VK_FORMAT_D16_UNORM_S8_UINT, 
                VK_FORMAT_D16_UNORM
            })
            {
                VkFormatProperties props;
                vkGetPhysicalDeviceFormatProperties(physical, format, &props);

                if(props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
                    return format;
            }

            return std::nullopt;
        }

        operator VkPhysicalDevice() const { return physical; }

        std::optional<uint32_t> queue_index(VkQueueFlags flags) const
        {
            // get queue indicies
            for(uint32_t i = 0; i < families.size(); i++)
                if((families[i].queueFlags & flags) == flags)
                    return i;

            return std::nullopt;
        }

        VkPhysicalDevice physical;
        VkPhysicalDeviceProperties props;
        VkPhysicalDeviceFeatures features;
        VkPhysicalDeviceMemoryProperties memprops;

        std::vector<VkExtensionProperties> extensions;
        std::vector<VkQueueFamilyProperties> families;
    };

    struct swapchain
    {
        swapchain(GLFWwindow* window)
        {
            
        }
    };
}

namespace volts::rsx::vulkan
{
    std::vector<physical_device> phsyical_devices(const VkInstance& instance)
    {
        uint32_t num = 0;
        VK_ENSURE(vkEnumeratePhysicalDevices(instance, &num, nullptr));

        if(num < 0)
            return {};

        std::vector<VkPhysicalDevice> devices(num);
        VK_ENSURE(vkEnumeratePhysicalDevices(instance, &num, devices.data()));

        return { std::begin(devices), std::end(devices) };
    }

    // we return by refence as its a static variable and can never go out of scope
    std::vector<VkLayerProperties>& layers()
    {
        // we only need to get this once, this is a static initalization trick 
        // it gets the compiler to do the work for us
        static std::vector<VkLayerProperties> props = []{
            uint32_t num = 0;
            vkEnumerateInstanceLayerProperties(&num, nullptr);

            std::vector<VkLayerProperties> lprops(num);
            vkEnumerateInstanceLayerProperties(&num, lprops.data());

            return std::move(lprops);
        }();

        return props;
    }

    bool has_layer(const std::string& name)
    {
        for(auto layer : layers())
            if(name == layer.layerName)
                return true;
        
        return false;
    }

    std::vector<VkExtensionProperties>& extensions()
    {
        static std::vector<VkExtensionProperties> exts = []{
            uint32_t num = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &num, nullptr);
            
            std::vector<VkExtensionProperties> exts(num);
            vkEnumerateInstanceExtensionProperties(nullptr, &num, exts.data());

            for(auto ext : exts)
                spdlog::info("supported extensions: {}", ext.extensionName);

            return exts;
        }();

        return exts;
    }

    bool has_extension(const std::string& name)
    {
        for(auto ext : extensions())
            if(name == ext.extensionName)
                return true;

        return false;
    }

    VkInstance instance(std::vector<std::string> extensions = {})
    {
        // create application info
        VkApplicationInfo appinfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
        appinfo.pApplicationName = "volts";
        appinfo.pEngineName = "emulated rsx";
        appinfo.apiVersion = VK_API_VERSION_1_0;
        
        // collect required extensions
        std::vector<const char*> exts;
        
        {
            // glfw requires some extensions so we query those
            uint32_t num = 0;
            const char** names = glfwGetRequiredInstanceExtensions(&num);
            exts.reserve(num);
            memcpy(exts.data(), names, num * sizeof(const char*));
        }

        spdlog::info("loading {} extra extensions", extensions.size());
        for(const auto& ext : extensions)
            if(!has_extension(ext))
                spdlog::error("unsupported extension {}", ext);
            else
                exts.push_back(ext.c_str());



        VkInstanceCreateInfo createinfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        createinfo.pApplicationInfo = &appinfo;
        
#if VK_VALIDATE
        // if validation is enabled then add the needed layers and extensions
        const char* layer_name = "VK_LAYER_KHRONOS_validation";
        V_ASSERT(has_layer(layer_name));
        
        createinfo.ppEnabledLayerNames = &layer_name;
        createinfo.enabledLayerCount = 1;


        V_ASSERT(has_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME));

        exts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        createinfo.enabledExtensionCount = exts.size();
        createinfo.ppEnabledExtensionNames = exts.data();


        // create instance
        VkInstance inst;
        VK_ENSURE(vkCreateInstance(&createinfo, nullptr, &inst));
        return inst;
    }
}

namespace volts::rsx::vulkan
{
    VkInstance instance(
        const std::string& name,
        const std::vector<std::string>& layers = {},
        const std::vector<std::string>& extensions = {})
    {
        VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
        
        appInfo.pApplicationName = name.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        
        appInfo.pEngineName = "emulated rsx";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkApplicationCreateInfo createInfo = { VK_STRUCTURE_TYPE_APPLICATION_CREATE_INFO };
        createInfo.pApplicationInfo = &appInfo;

        char** layerNames = alloca(sizeof(char*) * layers.size());
        char** extensionNames = alloca(sizeof(char*) * extensions.size());

        for(uint32_t i = 0; i < layers.size(); i++)
            layerNames[i] = layers[i].c_str();

        for(uint32_t i = 0; i < extensions.size(); i++)
            extensionNames[i] = extensions[i].c_str();

        // TODO: allocator and result type to handle errors
        VkInstance out;
        VK_ENSURE(vkCreateInstance(&createInfo, nullptr, &out));
        return out;
    }

    const std::vector<std::string>& extensions()
    {

    }

    const std::vector<std::string>& layers()
    {
        static auto layerNames = []{
            uint32_t num = 0;
            vkEnumerateInstanceLayerProperties(&num, nullptr);

            VkLayerProperties* props = alloca(sizeof(VkLayerProperties) * num);
            vkEnumerateInstanceLayerProperties(&num, props);

            std::vector<std::string> names(num);

            for(uint32_t i = 0; i < num; i++)
                names.push_back(props[i].layerName);

            return std::move(names);
        }();

        return layerNames;
    }
}