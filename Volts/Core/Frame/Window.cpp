#include "Window.h"

#include "imgui.h"

namespace Volts
{
    Window::Window()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        CreateFrame();
    }

    Window::~Window()
    {
        ImGui::DestroyContext();
    }
}