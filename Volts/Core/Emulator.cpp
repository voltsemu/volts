#include "Emulator.h"

namespace Volts
{
    // static initialization trick
    Emulator* Emulator::Get()
    {
        static Emulator E = {};
        return &E;
    }

    void Emulator::Run()
    {
        Window = GUI::Frame();
        Window.SetTitle("Volts");
        Window.Run();
    }

    bool ShowCPUOptions = true;
    void CPUOptions()
    {
        // this is empty for now
        ImGui::Begin("CPU Options", &ShowCPUOptions);
        ImGui::End();
    }

    bool ShowRenderOptions = true;
    void RenderOptions()
    {
        ImGui::Begin("Rendering options", &ShowRenderOptions);

        ImGui::End();
    }

    void Emulator::GUI()
    {

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