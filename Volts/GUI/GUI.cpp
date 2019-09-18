#include "Frame.h"

#include "imgui/imgui.h"

namespace Volts::GUI
{
    void Frame::GUILoop(Frame* F)
    {
        auto Now = std::chrono::high_resolution_clock::now();
        
        ImGui::SetNextWindowPos(ImVec2(25, 25), ImGuiCond_Once);
        ImGui::Begin("Metrics");
        ImGui::Text("FPS: %fms", ((double)std::chrono::duration_cast<std::chrono::nanoseconds>(Now - F->LastFrame).count())/1e6);
        ImGui::End();

        F->LastFrame = Now;
    }
}