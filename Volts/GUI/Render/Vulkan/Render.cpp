#include "Render.h"

#include "Frame.h"

namespace Volts::RSX
{

    static Vulkan* Singleton = new Vulkan();

    Vulkan::Vulkan()
    {
        GUI::Frame::GetSingleton()->AddRender(this);

        SetupVulkan();
    }

    Vulkan::~Vulkan()
    {
        DeleteInstance();
    }

    void Vulkan::Attach(GUI::Frame* Handle)
    {
        Frame = Handle;

        // create the surface
        CreateSurface();
    }

    void Vulkan::Detach()
    {
        DeleteSurface();
    }

    void Vulkan::BeginRender()
    {

    }

    void Vulkan::PresentRender()
    {

    }

    void Vulkan::Resize(GUI::Size NewSize)
    {

    }

    void Vulkan::Windowed()
    {

    }

    void Vulkan::Borderless()
    {

    }

    void Vulkan::Fullscreen()
    {

    }

    Device* Vulkan::Devices(U32* Count)
    {
        //*Count = DeviceCount;
        return nullptr;
    }

    void Vulkan::SetDevice(RSX::Device* Device)
    {
        // TODO
    }

    void Vulkan::UpdateVSync(bool NewMode)
    {
        // TODO
    }


    constexpr const char* ExtensionNames[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(VK_USE_PLATFORM_WIN32_KHR)
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
        VK_MVK_MACOS_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
        VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_XCB_KHR)
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

    constexpr const char* DeviceExtensionNames[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    constexpr U32 DeviceExtensionCount = sizeof(DeviceExtensionNames) / sizeof(const char*);

    void Vulkan::CreateInstance()
    {
        VkApplicationInfo AppInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
        AppInfo.pApplicationName = "Volts";
        AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        AppInfo.pEngineName = "RSX";
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

        VK_VALIDATE(vkCreateInstance(&CreateInfo, nullptr, &Instance));
    }

    void Vulkan::DeleteInstance()
    {
        vkDestroyInstance(Instance, nullptr);
    }

    void Vulkan::CreateDevices()
    {

    }

    void Vulkan::DeleteDevices()
    {

    }

    void Vulkan::CreateSurface()
    {
        // TODO: support all platforms
#if defined(VK_USE_PLATFORM_WIN32_KHR)
        VkWin32SurfaceCreateInfoKHR CreateInfo = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
        CreateInfo.hinstance = GUI::Instance;
        CreateInfo.hwnd = Frame->Handle;
        VK_VALIDATE(vkCreateWin32SurfaceKHR(Instance, &CreateInfo, nullptr, &Surface));
#elif defined(VK_USE_PLATFORM_MACOS_MVK)

#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)

#elif defined(VK_USE_PLATFORM_XCB_KHR)

#endif
    }

    void Vulkan::DeleteSurface()
    {
        vkDestroySurfaceKHR(Instance, Surface, nullptr);
    }

    void Vulkan::SetupVulkan()
    {
        // create the vulkan instance, we need this to do everything
        CreateInstance();
    }

    void Vulkan::CreatePipeline()
    {

    }
}