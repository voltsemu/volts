#include <Core/Collections/CthulhuString.h>
#include "Render.h"

namespace Volts::PS3::RSX
{
    using namespace Cthulhu;

    Vulkan::Vulkan()
    {
        Render::Register(this);
        VulkanSupport::InitExtensions();
        RenderInstance = VulkanSupport::MakeInstance();

        // if that failed then chances are the drivers dont work
        // because are we really going to run out of memory this
        // early on
        if(RenderInstance == nullptr)
            return;

        DeviceArray = VulkanSupport::ListDevices(RenderInstance);
    }

    Vulkan::~Vulkan()
    {
        vkDestroyInstance(RenderInstance, nullptr);
        delete DeviceArray;
    }

    InitError Vulkan::Init()
    {
        return InitError::Ok;
    }

    void Vulkan::DeInit()
    {

    }

    void Vulkan::Test()
    {
        RSX::Frame F;
        F.SetHeight(500);
        F.SetWidth(500);
        F.SetX(500);
        F.SetY(500);
        F.SetTitle("Skidaddle");
        F.InputHandle(DefWindowProc);
        F.Create();

        while(F.ShouldStayOpen())
            F.PollEvents();
    }

    RenderDevice* Vulkan::Devices(unsigned& Count) const
    {
        Count = DeviceArray->Len();
        return DeviceArray->Data();
    }

    bool Vulkan::Supported() const
    {
        return RenderInstance != nullptr;
    }

    static Vulkan* VKSingleton = new Vulkan();
}
