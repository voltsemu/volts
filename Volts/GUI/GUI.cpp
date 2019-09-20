#include "Frame.h"

#include "imgui/imgui.h"

#include "Core/Logger/Logger.h"

#include "PS3/Util/Decrypt/UNSELF.h"
#include <deque>
#include <string>

namespace Volts::GUI
{
    ImGuiTextBuffer Frame::Logs = {};
    void Frame::Log(std::string Msg)
    {
        Logs.append(Msg.c_str());
    }

    bool ShowMetric = true;

    void ShowMetrics(Frame* F)
    {
        auto Now = std::chrono::high_resolution_clock::now();
        ImGui::SetNextWindowPos(ImVec2(25, 25), ImGuiCond_FirstUseEver);
        ImGui::Begin("Metrics", &ShowMetric);
        ImGui::Text("FPS: %f.2ms", ((double)std::chrono::duration_cast<std::chrono::nanoseconds>(Now - F->LastFrame).count())/1e6);
        ImGui::End();

        F->LastFrame = Now;
    }

    bool ShowLog = true;
    void ShowLogs(Frame* F)
    {
        ImGui::Begin("Log", &ShowLog);

        if(ImGui::Button("Clear"))
            Frame::Logs.clear();

        ImGui::SameLine();

        if(ImGui::Button("Test"))
            Frame::Logs.append("Test\n");

        ImGui::Separator();

        ImGui::BeginChild("scrolling");
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 0));
        ImGui::TextUnformatted(Frame::Logs.begin());
        ImGui::PopStyleVar();

        ImGui::EndChild();
        ImGui::End();
    }

    void ShowSettings(Frame* F)
    {
        ImGui::SetNextWindowPos(ImVec2(200, 25), ImGuiCond_FirstUseEver);
        ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_MenuBar);

        if(ImGui::BeginMenuBar())
        {
            if(ImGui::BeginMenu("Windows"))
            {
                ImGui::MenuItem("Metrics", nullptr, &ShowMetric);
                ImGui::MenuItem("Logs", nullptr, &ShowLog);
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::End();
    }

    void Frame::GUILoop(Frame* F)
    {
        ShowSettings(F);
        if(ShowMetric) ShowMetrics(F);

        if(ShowLog) ShowLogs(F);

        ImGui::Begin("UNSELF");
        if(ImGui::Button("Do the thing"))
        {
            Cthulhu::FileSystem::BufferedFile F{"/Users/student/Documents/GitHub/Volts/Build/EBOOT.BIN"};
            VINFO("Decrypting file");
            auto I = PS3::UNSELF::DecryptSELF(F);
        }
        ImGui::End();

        ImGui::Begin("Debug");
        if(ImGui::Button("Fullscreen"))
            F->Fullscreen();

        if(ImGui::Button("Borderless"))
            F->Borderless();

        if(ImGui::Button("Windowed"))
            F->Windowed();

        ImGui::End();
    }
}