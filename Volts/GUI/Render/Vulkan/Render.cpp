#include "Render.h"

#include "Frame.h"

namespace Volts::RSX
{
    static Vulkan* Singleton = new Vulkan();

    Vulkan::Vulkan()
    {
        GUI::Frame::Renders.Append((void*)this);
    }

    Vulkan::~Vulkan()
    {

    }

    void Vulkan::Attach(GUI::Frame* Handle)
    {
        Frame = Handle;
        CreateInstance();
        QueryDevices();
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

    void Vulkan::CreateInstance()
    {
        VkApplicationInfo AppInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
        AppInfo.pApplicationName = "Volts";
        AppInfo.pEngineName = "RSX";
        AppInfo.apiVersion = VK_MAKE_VERSION(1, 0, 3);

        VkInstanceCreateInfo CreateInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };

        const char* Extensions[] = {
            VK_KHR_SURFACE_EXTENSION_NAME,
#if OS_WINDOWS
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif OS_LINUX
            VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#elif OS_APPLE
            VK_EXT_metal_surface,
#endif

#if VVKDEBUG
            VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#endif
        };

        CreateInfo.enabledExtensionCount = sizeof(Extensions) / sizeof(const char*);
        CreateInfo.ppEnabledExtensionNames = Extensions;

#if VVKDEBUG
        const char* Layers[] = { "VK_LAYER_LUNARG_standard_validation" };

        CreateInfo.enabledLayerCount = sizeof(Layers) / sizeof(const char*);
        CreateInfo.ppEnabledLayerNames = Layers;
#endif

        VK_VALIDATE(vkCreateInstance(&CreateInfo, nullptr, &Instance));
    }

    void Vulkan::QueryDevices()
    {
        U32 GPUCount = 0;
        VK_VALIDATE(vkEnumeratePhysicalDevices(Instance, &GPUCount, nullptr));

        VkPhysicalDevice* PhysicalDevices = alloca(sizeof(VkPhysicalDevice) * GPUCount);
        VK_VALIDATE(vkEnumeratePhysicalDevices(Instance, GPUCount, PhysicalDevices));

        for(U32 I = 0; I < GPUCount; I++)
            Devices.Append(VulkanSupport::VulkanDevice(PhysicalDevices[I]));

        SetDevice(Devices[0]);

        U32 Count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(CurrentDevice.PhysicalDevice, &Count, nullptr);
        VkQueueFamilyProperties* Queues = alloca(sizeof(VkQueueFamilyProperties) * Count);
        vkGetPhysicalDeviceQueueFamilyProperties(CuurentDevice.PhysicalDevice, &Count, Queues);
        for(U32 I = 0; I < Count; I++)
        {
            if(Queues[I].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                GraphicsIndex = I;
                break;
            }
        }
    }

    void Vulkan::SetDevice(VulkanSupport::VulkanDevice NewDevice)
    {
        CurrentDevice = NewDevice;
        const char* Extensions[] = {
            "VK_KHR_swapchain"
        };
        VkDeviceQueueCreateInfo QueueInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
        QueueInfo.queueFamilyIndex = QueueIndex;
        QueueInfo.queueCount = 1;
        const float Priority = 1.0f;
        QueueInfo.pQueuePriorities = &Priority;

        VkDeviceCreateInfo CreateInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
        CreateInfo.queueCreateInfoCount = 1;
        CreateInfo.pQueueCreateInfos = &QueueInfo;
        CreateInfo.enabledExtensionCount = sizeof(Extensions) / sizeof(const char*);
        CreateInfo.ppEnabledExtensionNames = Extensions;
        VK_VALIDATE(vkCreateDevice(CurrentDevice.PhysicalDevice, &CreateInfo, nullptr, &LogicalDevice));
        VK_VALIDATE(vkGetDeviceQueue(LogicalDevice, GraphicsIndex, 0, &Queue));

        VkDescriptorPoolSize PoolSizes[] = {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };

        VkDescriptorPoolCreateInfo PoolInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
        PoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        PoolInfo.maxSets = 1000 * (sizeof(PoolSizes) / sizeof(VkDescriptorPoolSize));
        PoolInfo.poolSizeCount = sizeof(PoolSizes) / sizeof(VkDescriptorPoolSize);
        PoolInfo.pPoolSizes = PoolSizes;
        VK_VALIDATE(vkCreateDescriptorPool(LogicalDevice, &PoolInfo, nullptr, &DescriptorPool));
    }
}