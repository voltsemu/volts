#include "Render.h"

#include "Frame.h"

namespace Volts::RSX
{
    static Vulkan* Singleton = new Vulkan();

    Vulkan::Vulkan()
    {
        GUI::Frame::GetSingleton()->AddRender(this);
    }

    Vulkan::~Vulkan()
    {

    }

    void Vulkan::Attach(GUI::Frame* Handle)
    {
        Frame = Handle;
    }

    void Vulkan::Detach()
    {

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
        // TODO
        return nullptr;
    }

    void Vulkan::UpdateVSync(bool NewMode)
    {
        // TODO
    }
#if 0
    // create and destroy instance

    void Vulkan::CreateInstance()
    {
        VkApplicationInfo AppInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
        AppInfo.pApplicationName = "Volts";
        AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        AppInfo.pEngineName = "RSX";
        AppInfo.engineVersion = 1;
        AppInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo CreateInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        CreateInfo.pApplicationInfo = &AppInfo;
        CreateInfo.enabledExtensionCount = VulkanSupport::Consts::ExtenionCount;
        CreateInfo.ppEnabledExtensionNames = VulkanSupport::Consts::ExtensionNames;

#if VVKDEBUG
        CreateInfo.enabledLayerCount = VulkanSupport::Consts::LayerCount;
        CreateInfo.ppEnabledLayerNames = VulkanSupport::Consts::LayerNames;
#endif

        VK_VALIDATE(vkCreateInstance(&CreateInfo, nullptr, &Instance));
    }

    void Vulkan::DestroyInstance()
    {
        vkDestroyInstance(Instance, nullptr);
    }

    // create and destroy all devices

    void Vulkan::CreateDevices()
    {
        VK_VALIDATE(vkEnumeratePhysicalDevices(Instance, &DeviceCount, nullptr));

        // TODO: report error
        if(!DeviceCount)
            return;

        DeviceList = new VulkanSupport::VulkanDevice[DeviceCount];
        auto* PhysicalDevices = (VkPhysicalDevice*)alloca(sizeof(VkPhysicalDevice) * DeviceCount);
        VK_VALIDATE(vkEnumeratePhysicalDevices(Instance, &DeviceCount, PhysicalDevices));

        for(U32 I = 0; I < DeviceCount; I++)
            DeviceList[I] = VulkanSupport::VulkanDevice(PhysicalDevices[I]);
    }

    void Vulkan::DestroyDevices()
    {
        delete[] DeviceList;
    }

    // create queue family props

    void Vulkan::CreateQueueProps(VulkanSupport::VulkanDevice Dev)
    {
        vkGetPhysicalDeviceQueueFamilyProperties(Dev.PhysicalDevice, &QueueFamilyCount, nullptr);
        if(!QueueFamilyCount)
            return;

        QueueProps = new VkQueueFamilyProperties[QueueFamilyCount];
        vkGetPhysicalDeviceQueueFamilyProperties(Dev.PhysicalDevice, &QueueFamilyCount, QueueProps);
        if(!QueueFamilyCount)
            return;

        bool Found = false;
        for(U32 I = 0; I < QueueFamilyCount; I++)
        {
            if(QueueProps[I].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                GraphicsQueueFamilyIndex = I;
                Found = true;
                break;
            }
        }
    }

    void Vulkan::DestroyQueueProps()
    {
        delete[] QueueProps;
    }

    // create and destroy logical device
    void Vulkan::CreateDevice(VulkanSupport::VulkanDevice Dev)
    {
        float Priority = 0.f;

        VkDeviceQueueCreateInfo QueueInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
        QueueInfo.queueCount = 1;
        QueueInfo.pQueuePriorities = &Priority;
        QueueInfo.queueFamilyIndex = GraphicsQueueFamilyIndex;

        VkDeviceCreateInfo DeviceInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
        DeviceInfo.queueCreateInfoCount = 1;
        DeviceInfo.pQueueCreateInfos = &QueueInfo;
        DeviceInfo.enabledExtensionCount = VulkanSupport::Consts::DeviceExtensionCount;
        DeviceInfo.ppEnabledExtensionNames = VulkanSupport::Consts::DeviceExtensionNames;

        VK_VALIDATE(vkCreateDevice(Dev.PhysicalDevice, &DeviceInfo, nullptr, &Device));
    }

    void Vulkan::DestroyDevice()
    {
        vkDestroyDevice(Device, nullptr);
    }

    void Vulkan::CreateCommandPool()
    {
        VkCommandPoolCreateInfo PoolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        PoolInfo.queueFamilyIndex = GraphicsQueueFamilyIndex;

        VK_VALIDATE(vkCreateCommandPool(Device, &PoolInfo, nullptr, &CommandPool));
    }

    void Vulkan::DestroyCommandPool()
    {
        vkDestroyCommandPool(CommandPool);
    }

    void Vulkan::CreateCommandBuffer()
    {
        VkCommandBufferAllocateInfo BufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        BufferInfo.commandPool = CommandPool;
        BufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        BufferInfo.commandBufferCount = 1;

        VK_VALIDATE(vkAllocateCommandBuffers(Device, &BufferInfo, &CommandBuffer));
    }

    void Vulkan::DestroyCommandBuffer()
    {
        vkFreeCommandBuffers(Device, CommandPool, 1, &CommandBuffer);
    }
#endif
}