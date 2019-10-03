#include "Render.h"

namespace Volts::RSX
{
    Metal::Metal()
    {
        GUI::Frame::GetSingleton()->AddRender(this);

        NSArray<id<MTLDevice>>* MTLDevices = MTLCopyAllDevices();
        DeviceCount = [MTLDevices count];

        DeviceList = new MetalSupport::MetalDevice[DeviceCount];

        for(U32 I = 0; I < DeviceCount; I++)
            DeviceList[I] = MetalSupport::MetalDevice(MTLDevices[I]);
    }

    void Metal::Attach(GUI::Frame* F)
    {
        Frame = F;
    }

    void Metal::Detach()
    {

    }

    Device* Metal::Devices(U32* Count)
    {
        *Count = DeviceCount;
        return DeviceList;
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

    void Metal::BeginRender(VIEW_ARG(V))
    {
        View = (__bridge MTKView*)V;
    }

    void Metal::PresentRender()
    {

    }
}