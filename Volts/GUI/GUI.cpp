#include "Frame.h"
#include "Render/Render.h"
#include "imgui/imgui.h"

#include "Core/Logger/Logger.h"

#include "PS3/Util/Decrypt/UNSELF.h"
#include <vector>
#include <cstdlib>
#include <locale>
#include <codecvt>
#include <iostream>
#include <fstream>

namespace Volts::GUI
{
    ImGuiTextBuffer Frame::Logs = {};
    void Frame::Log(std::string Msg)
    {
        Logs.append(Msg.c_str());
    }

    bool ShowMetrics = true;

    void HelpMarker(const char* Text)
    {
        ImGui::TextDisabled("(?)");
        if(ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.f);
            ImGui::TextUnformatted(Text);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    void Metrics(Frame* F)
    {
        auto Now = std::chrono::high_resolution_clock::now();
        ImGui::SetNextWindowPos(ImVec2(25, 25), ImGuiCond_FirstUseEver);
        ImGui::Begin("Metrics", &ShowMetrics);
        double FrameTime = (double)std::chrono::duration_cast<std::chrono::nanoseconds>(Now - F->LastFrame).count() / 1e6;
        ImGui::Text("FrameTime: %.1fms", FrameTime);
        ImGui::Text("FPS: %f", 1000 / FrameTime);
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

        static bool EnabledVsync = false;
        ImGui::Checkbox("VSync", &EnabledVsync);
        ImGui::SameLine(); HelpMarker("Change vertical sync options (vsync)");
        F->CurrentRender->UpdateVSync(EnabledVsync);

        U32 DeviceCount = 0;
        auto* Devs = Frame::CurrentRender->Devices(&DeviceCount);
        DeviceCount -= 1;
        std::vector<const char*> Names;
        for(U32 I = 0; I < DeviceCount; I++)
            Names.push_back(std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(Devs[I].Name()).c_str());

        static int CurrentDevice = 0;
        ImGui::Combo("device", &CurrentDevice, (const char**)Names.data(), DeviceCount);

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

    void Frame::GUILoop(Frame* F)
    {
        if(ShowSettings) Settings(F);
        if(ShowMetrics) Metrics(F);
        if(ShowOptions) Options(F);
        if(ShowLogs) LogWindow(F);
        if(static bool Demo = true; Demo) ImGui::ShowDemoWindow(&Demo);

        ImGui::Begin("UNSELF");
        if(ImGui::Button("Do the thing"))
        {
            Cthulhu::FileSystem::BufferedFile F{"/Users/student/Documents/GitHub/Volts/Build/EBOOT.BIN"};
            VINFO("Decrypting file");
            auto I = PS3::UNSELF::DecryptSELF(F).Get();
            auto Out = fopen("VOLTS.TXT", "w");
            for(U32 L = 0; L < I.Len(); L++)
                fprintf(Out, "%u ", I.GetData()[L]);
            
            fclose(Out);
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