#include "Emulator.h"

#include "debugapi.h"

namespace Volts
{
    Emulator* Emulator::Global = nullptr;
    // static initialization trick
    Emulator* Emulator::Get()
    {
        if(!Emulator::Global)
            Emulator::Global = new Emulator();

        return Emulator::Global;
    }

    void Emulator::Run()
    {
        Window = GUI::Frame();
        Window.SetTitle("Volts");
        Window.Run();
    }

    void Emulator::GUI()
    {

    }

    void Emulator::Log(Level L, std::string&& Message)
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
            LogBuffer.append(Message.c_str());
        }
    }

    void Emulator::Register(RSX::Render* Backend)
    {
        VINFO("Registered {} rendering backend", Backend->Name());
        RenderBackends.Append(Backend);
    }

    void Emulator::Register(Audio::Player* Backend)
    {
        VINFO("Registered {} audio backend", Backend->Name());
        AudioBackends.Append(Backend);
    }

    void Emulator::Register(Input::Controller* Backend)
    {
        VINFO("Registered {} input backend", Backend->Name());
        InputBackends.Append(Backend);
    }
}