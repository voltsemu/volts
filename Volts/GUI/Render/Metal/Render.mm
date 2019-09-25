#include "Render.h"

#include "imgui/examples/imgui_impl_osx.h"
#include "imgui/examples/imgui_impl_metal.h"

namespace Volts::RSX
{
    Metal::Metal()
        : IO(ImGui::GetIO())
    {
        GUI::Frame::Renders.Append((void*)this);
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
        IO = ImGui::GetIO();
    }

    void Metal::PresentRender()
    {

    }
}