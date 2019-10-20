#include "Vulkan.h"

namespace Volts::Render
{
    static Vulkan* Singleton = new Vulkan();

    Vulkan::Vulkan()
    {
        //Emulator::Get()->Render.Register(this);
    }

    const Device* Vulkan::Devices(U32* Count) const
    {
        *Count = 0;
        return nullptr;
    }

    void Vulkan::SetDevice(U32 Index)
    {

    }

    void Vulkan::Begin()
    {

    }

    void Vulkan::End()
    {

    }

    void Vulkan::Attach()
    {

    }

    void Vulkan::Detach()
    {

    }

    void Vulkan::Resize(U32 Width, U32 Height)
    {

    }
}