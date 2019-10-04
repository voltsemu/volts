#include "VulkanCommon.h"

#include "Core/Logger/Logger.h"

#if OS_WINDOWS
#   include "Libloaderapi.h"
#   define DLL_HANDLE HMODULE
#   define DLL_OPEN(Path) LoadLibraryA(Path)
#   define DLL_CLOSE(Lib) FreeLibrary(Lib)
#   define DLL_GET(Lib, Name) (PFN_##Name)GetProcAddress(Lib, #Name)
#   define DLL_VK_PATH "vulkan-1.dll"
#elif OS_LINUX
#   include <dlfcn.h>
#   define DLL_HANDLE void*
#   define DLL_OPEN(Path) dlopen(Path, RTLD_NOW)
#   define DLL_CLOSE(Lib) dlclose(Lib)
#   define DLL_GET(Lib, Name) (PFN_##Name)dlsym(Lib, #Name)
#   define DLL_VK_PATH "libvulkan.so.1"
#else
// apple doesnt need any of this because moltenvk ships with it
#endif

using namespace Cthulhu;

namespace Volts::RSX::Vk
{
    DLL_HANDLE Handle = nullptr;

    using PFN_vkGetInstanceProcAddr = PFN_vkVoidFunction(VKAPI_PTR*)(VkInstance Instance, const char* Name);
    PFN_vkGetInstanceProcAddr GetAddress = nullptr;


    constexpr const char* ExtensionNames[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(VK_USE_PLATFORM_WIN32_KHR)
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
        VK_KHR_MACOS_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
        VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
#else
        VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#endif
    };

    constexpr U32 ExtensionCount = sizeof(ExtensionNames) / sizeof(const char*);

#if VVKDEBUG
    constexpr const char* LayerNames[] = {
        "VK_LAYER_LUNARG_standard_validation"
    };

    constexpr U32 LayerCount = sizeof(LayerNames) / sizeof(const char*);
#endif

    constexpr const char* ApplicationName = "Volts";
    constexpr const char* EngineName = "RSX";


    constexpr const char* DeviceExtensionNames[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    constexpr U32 DeviceExtensionCount = sizeof(DeviceExtensionNames) / sizeof(const char*);


    PFN_vkCreateInstance vkCreateInstance = nullptr;
    PFN_vkDestroyInstance vkDestroyInstance = nullptr;
    VkInstance GlobalInstance;
    bool CreateGlobalInstance()
    {
        VkApplicationInfo AppInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
        AppInfo.pApplicationName = ApplicationName;
        AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        AppInfo.pEngineName = EngineName;
        AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        AppInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo CreateInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        CreateInfo.pApplicationInfo = &AppInfo;
        CreateInfo.enabledExtensionCount = ExtensionCount;
        CreateInfo.ppEnabledExtensionNames = ExtensionNames;
#if VVKDEBUG
        CreateInfo.enabledLayerCount = LayerCount;
        CreateInfo.ppEnabledLayerNames = LayerNames;
#endif
        return vkCreateInstance(&CreateInfo, nullptr, &GlobalInstance) == VK_SUCCESS;
    }

#define FUNC(Name) (PFN_##Name)GetAddress(nullptr, #Name)

    bool Load()
    {
        VINFO("Attempting to load vulkan")
        if(!(Handle = DLL_OPEN(DLL_VK_PATH)))
        {
            VWARN("Vulkan runtime missing")
            return false;
        }

        if(!(GetAddress = DLL_GET(Handle, vkGetInstanceProcAddr)))
        {
            VWARN("Vulkan runtime corrupted (vkGetInstanceProcAddr missing)")
            return false;
        }

        if(!(vkCreateInstance = (PFN_vkCreateInstance)GetAddress(nullptr, "vkCreateInstance")))
        {
            VWARN("Vulkan runtime corrupted (vkCreateInstance missing)");
            return false;
        }

        if(!CreateGlobalInstance())
        {
            VWARN("Vulkan runtime corrupted (failed to create instance)");
            return false;
        }

        VINFO("Loaded vulkan successfully");

        vkDestroyInstance = FUNC(vkDestroyInstance);

        return true;
    }

    void Unload()
    {
        vkDestroyInstance(GlobalInstance, nullptr);

        if(Handle)
            DLL_CLOSE(Handle);

        VINFO("Unloaded vulkan");
    }

    VkInstance Instance()
    {
        return GlobalInstance;
    }
}