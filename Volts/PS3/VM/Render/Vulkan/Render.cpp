#if WITH_VULKAN

#include <Core/Collections/CthulhuString.h>
#include "Render.h"

namespace Volts::PS3::RSX
{
    using namespace Cthulhu;

    Vulkan::Vulkan()
    {
        MessageBox(nullptr, "Vulkan Ctor", "Volts", 0);
    }

    InitError Vulkan::Init()
    {
        RenderInstance = VulkanSupport::MakeInstance();

        // if that failed then chances are the drivers dont work
        // because are we really going to run out of memory this
        // early on
        if(RenderInstance == nullptr)
            return InitError::NoDriver;

        // everything went well
        return InitError::Ok;
    }

    void Vulkan::DeInit()
    {
        vkDestroyInstance(RenderInstance, nullptr);
    }

    void Vulkan::Test()
    {
    }

    RenderDevice* Vulkan::Devices(unsigned& Count) const
    {
        Count = DeviceArray.Len();
        return *DeviceArray;
    }

    bool Vulkan::Supported() const
    {
        return true;
    }

    static Vulkan* VKSingleton = new Vulkan();
}

#endif // WITH_VULKAN