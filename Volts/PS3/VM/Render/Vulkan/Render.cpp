#if WITH_VULKAN

#include <Core/Collections/CthulhuString.h>
#include <vulkan/vulkan.h>
#include "Render.h"
#include "Volts/Core/Macros.h"

namespace Volts::PS3::RSX
{
    using namespace Cthulhu;

#define DECLARE(Name) CAT(PFN_, Name) Name

    DECLARE(vkEnumerateInstanceExtensionProperties);
    DECLARE(vkCreateInstance);
    DECLARE(vkEnumeratePhysicalDevices);
    DECLARE(vkGetPhysicalDeviceProperties);
    DECLARE(vkDestroyInstance);

#if OS_WINDOWS
#   define LOAD(Handle, Name) { Name = (decltype(Name))GetProcAddress(Handle, #Name); }
#elif OS_LINUX
#   define LOAD(Handle, Name) { Name = (decltype(Name))dlsym(Handle, #Name); }
#endif

    void Vulkan::Init()
    {
#if OS_WINDOWS
        VulkanDLL = LoadLibraryA("vulkan-1.dll");
#elif OS_LINUX
        VulkanDLL = dlopen("libvulkan.so.1");
#endif
        if(VulkanDLL == nullptr)
            return;

        LOAD(VulkanDLL, vkEnumerateInstanceExtensionProperties);
    }

#if OS_WINDOWS
    LRESULT CALLBACK WindowProc(HWND Wnd, UINT Msg, WPARAM W, LPARAM L)
    {
        switch(Msg)
        {
        case WM_KEYDOWN:
            break;
        case WM_KEYUP:
            break;
        default:
            return DefWindowProc(Wnd, Msg, W, L);
        }

        return 0;
    }
#endif

    void Vulkan::Test()
    {
        VkApplicationInfo AppInfo = {};
        AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        AppInfo.pApplicationName = "Volts";
        AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        AppInfo.pEngineName = "Reality";
        AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        AppInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo CreateInfo = {};
        CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        CreateInfo.pApplicationInfo = &AppInfo;
        CreateInfo.enabledExtensionCount = 0;
        CreateInfo.ppEnabledExtensionNames = nullptr;
        CreateInfo.enabledLayerCount = 0;
        MessageBox(nullptr, "Hello0", "Vulkan", 0);

        VkInstance Instance;
        vkCreateInstance(&CreateInfo, nullptr, &Instance);

        {
        //    MessageBox(nullptr, "Oh No", "Vulkan Go Boom", 0);
        //    return;
        }

        MessageBox(nullptr, "Hello1", "Vulkan", 0);
        Cthulhu::U32 DeviceCount = 0;
        vkEnumeratePhysicalDevices(Instance, &DeviceCount, nullptr);

        if(DeviceCount == 0)
        {
            MessageBox(nullptr, "Nothing supports vulkan", "Vulkan go boom", 0);
            return;
        }

        MessageBox(nullptr, "Hello2", "Vulkan", 0);

        VkPhysicalDevice* Devices = new VkPhysicalDevice[DeviceCount];
        vkEnumeratePhysicalDevices(Instance, &DeviceCount, Devices);

        MessageBox(nullptr, "Here", "Vulkan", 0);
        for(U32 I = 0; I < DeviceCount; I++)
        {
            VkPhysicalDeviceProperties Props;
            vkGetPhysicalDeviceProperties(Devices[I], &Props);

            MessageBox(nullptr, Props.deviceName, "Vulkan Device", 0);
        }

        RSX::Frame F;
        F.SetX(100);
        F.SetY(100);
        F.SetWidth(500);
        F.SetHeight(500);
        F.SetTitle("Vulkan");

#if OS_WINDOWS
        F.InputHandle(WindowProc);
#endif

        F.Create();

        while(F.ShouldStayOpen())
            F.PollEvents();

        vkDestroyInstance(Instance, nullptr);
    }

    bool Vulkan::Supported() const
    {
        return VulkanDLL != nullptr;
    }
}

#endif // WITH_VULKAN