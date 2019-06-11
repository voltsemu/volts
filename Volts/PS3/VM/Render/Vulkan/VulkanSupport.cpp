#include "VulkanSupport.h"

#if OS_WINDOWS
#   define VKLOAD(Handle, Name) { Name = (decltype(Name))GetProcAddress(Handle, #Name); }
#elif OS_LINUX
#   define VKLOAD(Handle, Name) { Name = (decltype(Name))dlsym(Handle, #Name); }
#endif

#define VKDECLARE(Name) CAT(PFN_, Name) Name

namespace Volts::PS3::RSX::VulkanSupport
{
    VKDECLARE(vkEnumerateInstanceExtensionProperties);
    VKDECLARE(vkCreateInstance);
    VKDECLARE(vkEnumeratePhysicalDevices);
    VKDECLARE(vkGetPhysicalDeviceProperties);
    VKDECLARE(vkDestroyInstance);

    using namespace Cthulhu;
    // on windows DLL files are HMODULES
    // on linux they're void*
#if OS_WINDOWS
    using DLL = HMODULE;
#elif OS_LINUX
    using DLL = void*;
#endif
    DLL VulkanDLL;

    bool Valid = false;

    bool LoadDLL()
    {
        // load the library in at runtime
#if OS_WINDOWS
        VulkanDLL = LoadLibraryA("vulkan-1.dll");
#elif OS_LINUX
        VulkanDLL = dlopen("libvulkan.so.1");
#endif

        return (Valid = VulkanDLL != nullptr);
    }

    bool Found()
    {
        return Valid;
    }

    void LoadFunctions()
    {
        VKLOAD(VulkanDLL, vkEnumerateInstanceExtensionProperties);
        VKLOAD(VulkanDLL, vkCreateInstance);
        VKLOAD(VulkanDLL, vkEnumeratePhysicalDevices);
        VKLOAD(VulkanDLL, vkGetPhysicalDeviceProperties);
        VKLOAD(VulkanDLL, vkDestroyInstance);
    }

    Array<VkExtensionProperties> Extensions;

    bool InitExtensions()
    {
        U32 Count;
        if(VulkanSupport::vkEnumerateInstanceExtensionProperties(nullptr, &Count, nullptr) != VK_SUCCESS)
            return false;

        Extensions.Reserve(Count);
        VulkanSupport::vkEnumerateInstanceExtensionProperties(nullptr, &Count, Extensions.Data());

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
}

#undef VKDECLARE
#undef VKLOAD