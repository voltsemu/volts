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

    Frame::Frame()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
    }

    Frame::~Frame()
    {
        ImGui::DestroyContext();
    }

    Frame* Frame::SetTitle(const char* T)
    {
        Title = T;
        return this;
    }

    const char* Frame::GetTitle() const
    {
        return Title;
    }

    void Frame::Log(Level L, const char* Msg)
    {
        if(L >= CurrentLevel)
        {
            switch(L)
            {
                case Level::Info:
                    LogBuffer.append("[info] ");
                    break;
                case Level::Warning:
                    LogBuffer.append("[warn] ");
                    break;
                case Level::Error:
                    LogBuffer.append("[error] ");
                    break;
                case Level::Fatal:
                    LogBuffer.append("[fatal] ");
                    break;
                default:
                    LogBuffer.append("[other] ");
                    break;
            }
            LogBuffer.append(Msg);
        }
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
        VINFO("Registered {} backend", R->Name());
        Renders.Append(R);
    }

    void Frame::SetRender(const char* Name)
    {
        for(U32 I = 0; I < RenderCount; I++)
        {
            if(strcmp(RenderNames[I], Name) == 0)
            {
                RenderIndex = I;
                CurrentRender = Renders[I];
                CurrentRender->Attach(this);
                VINFO("Attached {} as the current renderer", CurrentRender->Name());
                break;
            }
        }
    }

    void Frame::UpdateDevices()
    {
        if(!CurrentRender)
            return;

        delete[] DeviceNames;
        auto* Devices = CurrentRender->Devices(&DeviceCount);
        
        if(!Devices)
        {
            DeviceNames = nullptr;
            return;
        }

        DeviceNames = new const char*[DeviceCount]();
        for(U32 I = 0; I < DeviceCount; I++)
            DeviceNames[I] = Devices[I].Name();
    }
}