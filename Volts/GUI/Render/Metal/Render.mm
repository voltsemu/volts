#include "Render.h"

namespace Volts::RSX
{
    static Metal* Singleton = new Metal();
    Metal::Metal()
    {
        GUI::Frame::GetSingleton()->AddRender(this);

        NSArray<id<MTLDevice>>* MTLDevices = MTLCopyAllDevices();
        DeviceCount = [MTLDevices count];

        DeviceList = new MetalSupport::MetalDevice[DeviceCount];

        for(U32 I = 0; I < DeviceCount; I++)
            DeviceList[I] = MetalSupport::MetalDevice(MTLDevices[I]);

        // TODO
    }

    void Metal::Attach(GUI::Frame* F)
    {
        Frame = F;
        ImGui_ImplMetal_Init(CurrentDevice());
    }

    void Metal::Detach()
    {
        ImGui_ImplMetal_Shutdown();
    }

    Device* Metal::Devices(U32* Count)
    {
        *Count = DeviceCount;
        return DeviceList;
    }

    void Metal::SetDevice(RSX::Device* Device)
    {
        for(U32 I = 0; I < DeviceCount; I++)
        {
            if(strcmp(DeviceList[I].Name(), Device->Name()) == 0)
            {
                DeviceIndex = I;
                break;
            }
        }
    }

    void Metal::UpdateVSync(bool NewMode)
    {

    }

    void Metal::Windowed()
    {

    }

    void Metal::Fullscreen()
    {

    }

    void Metal::Borderless()
    {

    }

    void Metal::Resize(GUI::Size NewSize)
    {

    }

    void Metal::BeginRender()
    {
        // TODO
    }

    void Metal::PresentRender()
    {
        // TODO
    }

    void Metal::CreatePipelineState()
    {
        // TODO
    }
}