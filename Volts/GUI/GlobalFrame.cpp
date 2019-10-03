#include "Frame.h"

#include "Render/Render.h"
#include "Render/DX12/Render.h"

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

    Frame::Frame() {}

    Frame* Frame::SetTitle(const char* T)
    {
        Title = T;
        return this;
    }

    const char* Frame::GetTitle() const
    {
        return Title;
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

    void Frame::AddRender(RSX::Render* R)
    {
        Renders.Append(R);
    }

    void Frame::SetRender(const char* Name)
    {
        CurrentRender = new RSX::DX12(); //Renders[1];
        CurrentRender->Attach(this);
    }

    void Frame::FinalizeDevices()
    {
        delete[] DeviceNames;
        if(!!CurrentRender)
        {
            auto* Devices = CurrentRender->Devices(&DeviceCount);
            if(!!Devices)
                return;

            DeviceNames = new const char*[DeviceCount]();
            for(U32 I = 0; I < DeviceCount; I++)
                DeviceNames[I] = "";
        }
    }
}