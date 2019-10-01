#include "Render.h"

#include "imgui/examples/imgui_impl_osx.h"
#include "imgui/examples/imgui_impl_metal.h"

#import <Metal/Metal.h>

namespace Volts::RSX
{
    Metal::Metal()
    {
        GUI::Frame::GetSingleton()->Renders.Append(this);

        NSArray<id<MTLDevice>>* MTLDevices = MTLCopyAllDevices();

        DeviceList = new MetalSupport::MetalDevice[[MTLDevices count]];

        for(U32 I = 0; I < [MTLDevices count]; I++)
            DeviceList[I] = MetalSupport::MetalDevice((__bridge void*)MTLDevices[I]);
    }

    void Metal::Attach(GUI::Frame* F)
    {
        Frame = F;
    }

    void Metal::Detach()
    {
        ImGui_ImplMetal_Shutdown();
        ImGui_ImplOSX_Shutdown();
    }

    Device* Metal::Devices(U32* Count)
    {
        return nullptr;
    }

    void Metal::UpdateVSync(bool NewMode) {}

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

    }

    void Metal::PresentRender()
    {

    }
}