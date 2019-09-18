#include "Frame.h"

#include "imgui/imgui.h"

#include "Core/Logger/Logger.h"

#include "PS3/Util/Decrypt/UNSELF.h"
#include <deque>
#include <string>

namespace Volts::GUI
{
    ImGuiTextBuffer Logs;

    void Frame::Log(std::string Msg)
    {
        Logs.append(Msg.c_str());
    }

    bool Metrics = true;

    void ShowMetrics(Frame* F)
    {
        auto Now = std::chrono::high_resolution_clock::now();
        if(Metrics)
        {
            ImGui::SetNextWindowPos(ImVec2(25, 25), ImGuiCond_FirstUseEver);
            ImGui::Begin("Metrics", &Metrics);
            ImGui::Text("FPS: %f.2ms", ((double)std::chrono::duration_cast<std::chrono::nanoseconds>(Now - F->LastFrame).count())/1e6);
            ImGui::End();
        }

        F->LastFrame = Now;
    }

    void ShowSettings(Frame* F)
    {
        ImGui::SetNextWindowPos(ImVec2(200, 25), ImGuiCond_FirstUseEver);
        ImGui::Begin("Settings");
        if(ImGui::BeginMenuBar())
        {
            if(ImGui::BeginMenu("Windows"))
            {
                ImGui::MenuItem("Metrics", nullptr, &Metrics);
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::End();
    }

    void Frame::GUILoop(Frame* F)
    {
        ShowSettings(F);
        ShowMetrics(F);
        
        ImGui::Begin("Log");
        
        if(ImGui::Button("Clear"))
            Logs.clear();

        ImGui::SameLine();

        if(ImGui::Button("Test"))
            Logs.append("Test\n");

        ImGui::Separator();

        ImGui::BeginChild("scrolling");
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 0));
        ImGui::TextUnformatted(Logs.begin());
        ImGui::PopStyleVar();

        ImGui::EndChild();
        ImGui::End();
    }
}