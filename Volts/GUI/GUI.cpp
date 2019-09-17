#include "Frame.h"

#include "imgui/imgui.h"

namespace Volts::GUI
{
    void Frame::GUILoop()
    {
        ImGui::Begin("Jeff");
        ImGui::Text("AAA");
        ImGui::End();
    }
}