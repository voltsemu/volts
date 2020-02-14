#include <vulkan/vulkan.h>

#define VK_ENSURE(expr) { if(VkResult res = (expr); res != VK_SUCCESS) { spdlog::error("vk error {} = {}", #expr, res); } }

#include <platform.h>

#if SYS_WINDOWS
#   include <Windows.h>
#endif

#include <GLFW/glfw3.h>

#include <vector>
#include <string>

#include <spdlog/spdlog.h>

namespace volts::rsx::vulkan
{
    std::vector<VkPhysicalDevice> phsyical_devices(const VkInstance& instance)
    {
        uint32_t num = 0;
        VK_ENSURE(vkEnumeratePhysicalDevices(instance, &num, nullptr));

        if(num < 0)
            return {};

        std::vector<VkPhysicalDevice> devices(num);
        VK_ENSURE(vkEnumeratePhysicalDevices(instance, &num, devices.data()));

        return std::move(devices);
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
            for(int i = 0; i < num; i++)
                exts.push_back(names[i]);
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
        const char* layer_name = "VK_LAYER_KHRONOS_validation";
        if(has_layer(layer_name))
        {
            createinfo.ppEnabledLayerNames = &layer_name;
            createinfo.enabledLayerCount = 1;
        }
        else
        {
            spdlog::error("validation was enabled but not supported");
        }

        exts.push_back("VK_EXT_debug_utils");
#endif

        createinfo.enabledExtensionCount = exts.size();
        createinfo.ppEnabledExtensionNames = exts.data();

        VkInstance inst;
        VK_ENSURE(vkCreateInstance(&createinfo, nullptr, &inst));
        return inst;
    }
}