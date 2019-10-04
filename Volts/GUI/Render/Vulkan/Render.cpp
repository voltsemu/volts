#include "Render.h"

#include "Frame.h"

namespace Volts::RSX
{
    static Vulkan* Singleton = new Vulkan();

    Vulkan::Vulkan()
    {
        GUI::Frame::GetSingleton()->AddRender(this);
        SupportFound = Vk::Load();
    }

    Vulkan::~Vulkan()
    {
        Vk::Unload();
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
        //*Count = DeviceCount;
        return nullptr;
    }

    void Vulkan::UpdateVSync(bool NewMode)
    {
        // TODO
    }
}