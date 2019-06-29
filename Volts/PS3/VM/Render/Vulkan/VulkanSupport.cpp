#include "VulkanSupport.h"

namespace Volts::PS3::RSX
{
    VulkanDevice::VulkanDevice(VkPhysicalDevice D)
        : Device(D)
    {
        vkGetPhysicalDeviceProperties(Device, &Properties);
    }

    std::wstring VulkanDevice::Name() const
    {
        return std::wstring(&Properties.deviceName[0], &Properties.deviceName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE]);
    }
}

namespace Volts::PS3::RSX::VulkanSupport
{
    using namespace Cthulhu;

    const char* ExtensionNames[] = {
            VK_KHR_SURFACE_EXTENSION_NAME,
#if OS_WINDOWS
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif OS_LINUX
            VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
#endif

#if VVKDEBUG
            "VK_LAYER_CHRONOS_validation",
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
        };

#if VVKDEBUG
    static VKAPI_ATTR VkBool32 VKAPI_CALL VkCallback(
        VkDebugUtilsMessageSeverityFlagsBitEXT Severity,
        VkDebugUtilsMessageTypeFlagsEXT Type,
        const VkDebugUtilsMessageCallbackDataEXT* Data,
        void* UserData
    )
    {
        if(Severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
            LOGF_DEBUG(VK, "Validation Layer: %s", Data->pMessage);

        return VK_FALSE;
    }
#endif

    void MakeInfo(VkApplicationInfo& App, VkInstanceCreateInfo& Create)
    {
        App.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        App.pApplicationName = "Volts";
        App.applicationVersion = VK_MAKE_VERSION(VMAJOR, VMINOR, VPATCH);
        App.pEngineName = "RSX";
        App.engineVersion = VK_MAKE_VERSION(VMAJOR, VMINOR, VPATCH);
        App.apiVersion = VK_API_VERSION_1_0;

        Create.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        Create.pApplicationInfo = &App;

        Create.enabledLayerCount = 0;
        Create.ppEnabledLayerNames = nullptr;
    }

    VkInstance MakeInstance()
    {
        VkApplicationInfo App = {};
        VkInstanceCreateInfo Create = {};
        MakeInfo(App, Create);

        Create.enabledExtensionCount = (sizeof(ExtensionNames) / sizeof(const char*));
        Create.ppEnabledExtensionNames = ExtensionNames;

        VkInstance Instance;
        if(vkCreateInstance(&Create, nullptr, &Instance) != VK_SUCCESS)
        {
            // TODO: log error
            return nullptr;
        }

        return Instance;
    }

    VkInstance SimpleInstance()
    {
        VkApplicationInfo App = {};
        VkInstanceCreateInfo Create = {};
        MakeInfo(App, Create);

        Create.enabledExtensionCount = 0;
        Create.ppEnabledExtensionNames = nullptr;

        VkInstance Instance;
        if(vkCreateInstance(&Create, nullptr, &Instance) != VK_SUCCESS)
        {
            // TODO: log error
            return nullptr;
        }

        return Instance;
    }

    Array<VkExtensionProperties> Extensions;

    bool InitExtensions()
    {
        U32 Count;
        if(vkEnumerateInstanceExtensionProperties(nullptr, &Count, nullptr) != VK_SUCCESS)
            return false;

        Extensions.Reserve(Count);
        vkEnumerateInstanceExtensionProperties(nullptr, &Count, Extensions.Data());

        return true;
    }

    bool Supported(const char* Name)
    {
        for(auto Ext : Extensions)
        {
            if(CString::Compare(Ext.extensionName, Name) == 0)
                return true;
        }

        return false;
    }

    Cthulhu::Array<VulkanDevice>* ListDevices(VkInstance Instance)
    {
        U32 Count = 0;
        if(vkEnumeratePhysicalDevices(Instance, &Count, nullptr) != VK_SUCCESS)
            return {};

        if(Count == 0)
            return {};

        auto* Devices = new VkPhysicalDevice[Count];

        vkEnumeratePhysicalDevices(Instance, &Count, Devices);

        Array<VulkanDevice>* Ret = new Array<VulkanDevice>();

        for(U32 I = 0; I < Count; I++)
            Ret->Append(Devices[I]);

        delete[] Devices;

        return Ret;
    }
}
