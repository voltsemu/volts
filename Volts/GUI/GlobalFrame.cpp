#include "Frame.h"

#include "Render/Render.h"

namespace Volts::GUI
{
    Frame* Frame::Singleton = nullptr;
    Frame* Frame::GetSingleton()
    {
        if(Frame::Singleton)
            return Frame::Singleton;

        Frame::Singleton = new Frame();
        return Frame::Singleton;
    }

    void Frame::Log(std::string Msg)
    {
        LogBuffer.append(Msg.c_str());
    }

    void Frame::FinalizeRenders()
    {
        RenderCount = Renders.Len();
        RenderNames = new const char*[RenderCount];
        for(U32 I = 0; I < RenderCount; I++)
            RenderNames[I] = Renders[I]->Name();
    }

    void Frame::FinalizeDevices()
    {
        delete[] DeviceNames;
        auto* Devices = CurrentRender->Devices(&DeviceCount);
        if(!!Devices)
            return;

        DeviceNames = new const char*[DeviceCount]();
        for(U32 I = 0; I < DeviceCount; I++)
            DeviceNames[I] = "";
    }
}