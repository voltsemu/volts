#include "Frame.h"

#include "imgui/imgui.h"

namespace Volts::GUI
{
    void Frame::GUILoop(Frame* F)
    {
        auto Now = std::chrono::high_resolution_clock::now();
        ImGui::Begin("Metrics");

        ImGui::Text("FPS: %lf", std::chrono::duration<double>(Now - F->LastFrame));
        ImGui::End();

        F->LastFrame = Now;
    }
}