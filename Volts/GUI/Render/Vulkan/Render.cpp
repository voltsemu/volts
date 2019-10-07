#include "Render.h"

#include "Frame.h"

namespace Volts::RSX
{
    static Vulkan* Singleton = new Vulkan();

    Vulkan::Vulkan()
    {
        Init();

        if(DeviceCount > 0)
            GUI::Frame::GetSingleton()->AddRender(this);
        else
            VWARN("No vulkan devices detected. Disabling vulkan");
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

#if VVKDEBUG
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
    };

    constexpr U32 ExtensionCount = sizeof(ExtensionNames) / sizeof(const char*);

#if VVKDEBUG
    constexpr const char* LayerNames[] = {
        "VK_LAYER_KHRONOS_validation"
    };

    constexpr U32 LayerCount = sizeof(LayerNames) / sizeof(const char*);
#endif

    constexpr const char* DeviceExtensionNames[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    constexpr U32 DeviceExtensionCount = sizeof(DeviceExtensionNames) / sizeof(const char*);

#if VVKDEBUG && !OS_APPLE
    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT Severity,
        VkDebugUtilsMessageTypeFlagsEXT Type,
        const VkDebugUtilsMessengerCallbackDataEXT* CallbackData,
        void* UserData
    )
    {
        if(Severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
            printf("Validation layer: %s\n", CallbackData->pMessage);

        return VK_FALSE;
    }
#endif

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

        // moltenvk doesnt support this
#if VVKDEBUG && !OS_APPLE
        VkDebugUtilsMessengerCreateInfoEXT DebugInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        DebugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        DebugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        DebugInfo.pfnUserCallback = DebugCallback;
        CreateInfo.pNext = &DebugInfo;
#endif

        VK_VALIDATE(vkCreateInstance(&CreateInfo, nullptr, &Instance));
    }

    void Vulkan::DeleteInstance()
    {
#if VVKDEBUG && !OS_APPLE
        vkDestroyDebugUtilsMessengerEXT(Instance, DebugMessenger, nullptr);
#endif
        vkDestroyInstance(Instance, nullptr);
    }

    void Vulkan::CreateDevices()
    {
        vkEnumeratePhysicalDevices(Instance, &DeviceCount, nullptr);
        auto* PhysicalDevices = (VkPhysicalDevice*)alloca(sizeof(VkPhysicalDevice) * DeviceCount);
        vkEnumeratePhysicalDevices(Instance, &DeviceCount, PhysicalDevices);

        for(U32 I = 0; I < DeviceCount; I++)
        {
            VkPhysicalDeviceProperties DeviceProps;
            vkGetPhysicalDeviceProperties(PhysicalDevices[I], &DeviceProps);
        }
    }

    void Vulkan::DeleteDevices()
    {

    }

    void Vulkan::CreateLogicalDevice(VulkanSupport::VulkanDevice Dev)
    {
        PhysicalDevice = Dev.PhysicalDevice;
    }

    void Vulkan::DeleteLogicalDevice()
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
        VkMacOSSurfaceCreateInfoMVK CreateInfo = { VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK };
        CreateInfo.pView = [[(__bridge NSWindow*)Frame->Handle contentView] layer];
        VK_VALIDATE(vkCreateMacOSSurfaceMVK(Instance, &CreateInfo, nullptr, &Surface));
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
        VkWaylandSurfaceCreateInfoKHR CreateInfo = { VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR };
        // TODO
        // CreateInfo.display;
        // CreateInfo.surface;
        VK_VALIDATE(vkCreateWaylandSurfaceKHR(Instance, &CreateInfo, nullptr, &Surface));
#elif defined(VK_USE_PLATFORM_XCB_KHR)
        VkXcbSufraceCreateInfoKHR CreateInfo = { VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR };
        // TODO
        // CreateInfo.connection;
        // CreateInfo.window;
#endif
    }

    void Vulkan::DeleteSurface()
    {
        vkDestroySurfaceKHR(Instance, Surface, nullptr);
    }

    void Vulkan::CreateQueue()
    {

    }

    void Vulkan::DeleteQueue()
    {

    }

    void Vulkan::CreateSwapchain()
    {

    }

    void Vulkan::DeleteSwapchain()
    {

    }

    void Vulkan::Init()
    {
        // create the vulkan instance, we need this to do everything
        CreateInstance();

        CreateDevices();
    }

    void Vulkan::CreatePipeline()
    {

    }
}