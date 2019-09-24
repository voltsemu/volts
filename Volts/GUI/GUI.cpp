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

    bool ShowMetrics = true;

    void Metrics(Frame* F)
    {
        auto Now = std::chrono::high_resolution_clock::now();
        ImGui::SetNextWindowPos(ImVec2(25, 25), ImGuiCond_FirstUseEver);
        ImGui::Begin("Metrics", &ShowMetrics);
        ImGui::Text("FPS: %f.2ms", ((double)std::chrono::duration_cast<std::chrono::nanoseconds>(Now - F->LastFrame).count())/1e6);
        ImGui::End();

        F->LastFrame = Now;
    }

    bool ShowLogs = true;
    void LogWindow(Frame* F)
    {
        ImGui::Begin("Log", &ShowLogs);

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

    bool ShowOptions = false;
    void Options(Frame* F)
    {
        ImGui::Begin("Options", &ShowOptions);

        static const char* VSyncOptions[] = { "Disabled", "Single Buffered", "Double Buffered", "Triple Buffered" };
        static const char* CurrentItem = nullptr;
        if(ImGui::BeginCombo("##vsync", "VSync"))
        {
            for(U32 I = 0; I < IM_ARRAYSIZE(VSyncOptions); I++)
            {
                bool IsSelected = (CurrentItem = VSyncOptions[I]);
                if(ImGui::Selectable(VSyncOptions[I], IsSelected))
                    CurrentItem = VSyncOptions[I];
                if(IsSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::End();
    }

    bool ShowSettings = true;
    void Settings(Frame* F)
    {
        ImGui::SetNextWindowPos(ImVec2(200, 25), ImGuiCond_FirstUseEver);
        ImGui::Begin("Settings", &ShowSettings, ImGuiWindowFlags_MenuBar);

        if(ImGui::BeginMenuBar())
        {
            if(ImGui::BeginMenu("Windows"))
            {
                ImGui::MenuItem("Metrics", nullptr, &ShowMetrics);
                ImGui::MenuItem("Logs", nullptr, &ShowLogs);
                ImGui::MenuItem("Options", nullptr, &ShowOptions);
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::End();
    }

    bool Demo = true;

    void Frame::GUILoop(Frame* F)
    {
        if(ShowSettings) Settings(F);
        if(ShowMetrics) Metrics(F);
        if(ShowOptions) Options(F);
        if(ShowLogs) LogWindow(F);
        if(Demo) ImGui::ShowDemoWindow();

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