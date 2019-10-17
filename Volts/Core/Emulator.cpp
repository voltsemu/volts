#include "Emulator.h"

#include <iostream>
#include <filesystem>
#include <chrono>

#include "imgui.h"
#include "imgui/examples/imgui_impl_glfw.h"
#include "imfilebrowser.h"

namespace Volts
{
    bool LogScrollToBottom = false;

    void Info(const char* M)
    {
        auto* Emu = Emulator::Get();
        if(Emu->Level > LogLevel::Info)
            return;

        Emu->LogBuffer.append(fmt::format("[info] {}\n", M).c_str());
        LogScrollToBottom = true;
    }

    void Warn(const char* M)
    {
        auto* Emu = Emulator::Get();
        if(Emu->Level > LogLevel::Warn)
            return;

        Emu->LogBuffer.append(fmt::format("[info] {}\n", M).c_str());
        LogScrollToBottom = true;
    }

    void Error(const char* M)
    {
        auto* Emu = Emulator::Get();
        if(Emu->Level > LogLevel::Error)
            return;

        Emu->LogBuffer.append(fmt::format("[error] {}\n", M).c_str());
        LogScrollToBottom = true;
    }

    void Fatal(const char* M)
    {
        Emulator::Get()->LogBuffer.append(fmt::format("[fatal] {}\n", M).c_str());
        LogScrollToBottom = true;
    }

    using namespace std;
    using namespace rapidjson;
    Emulator* Emulator::Get()
    {
        static Emulator* Singleton = new Emulator();
        return Singleton;
    }

    using TimePoint = decltype(std::chrono::high_resolution_clock::now());
    using TimeDiff = std::chrono::duration<double, std::milli>;

    ImGui::FileBrowser FileDialog;
    TimePoint LastFrame = std::chrono::high_resolution_clock::now();
    void Emulator::GUI()
    {
        ImGui::Begin("Metrics");
        {
            auto Now = std::chrono::high_resolution_clock::now();

            TimeDiff Count = (Now - LastFrame);

            ImGui::Text("%s", fmt::format("FrameTime: {:.2f}ms", Count.count()).c_str());
            ImGui::Text("%s", fmt::format("FPS: {:.2f}", 1000 / Count.count()).c_str());

            LastFrame = Now;
        }
        ImGui::End();

        ImGui::Begin("Backends");
        {
            ImGui::Combo("Renders", &Render.Index, Render.Names, Render.Count);

            static bool VSyncEnabled = true;
            ImGui::Checkbox("VSync", &VSyncEnabled);

            // Render.Current()->SetVSync(VSyncEnabled);

            ImGui::Combo("Audio", &Audio.Index, Audio.Names, Audio.Count);
            ImGui::Combo("Input", &Input.Index, Input.Names, Input.Count);
        }
        ImGui::End();

        ImGui::Begin("Logs");
        {
            const char* LevelOptions[] = { "Info", "Warn", "Error", "Fatal" };
            ImGui::Combo("Filter", (I32*)&Level, LevelOptions, IM_ARRAYSIZE(LevelOptions));

            ImGui::SameLine();
            if(ImGui::Button("Clear"))
                LogBuffer.clear();

            ImGui::Separator();
            ImGui::BeginChild("LogBox");
            ImGui::TextUnformatted(LogBuffer.c_str());
            
            if(LogScrollToBottom)
                ImGui::SetScrollHere(1.f);
            LogScrollToBottom = false;

            ImGui::EndChild();
        }
        ImGui::End();

        ImGui::Begin("Decrypt");
        {
            if(ImGui::Button("Decrypt EBOOT"))
            {
                FileDialog.SetTitle("UNSELF");
                FileDialog.Open();
            }

            FileDialog.Display();

            if(FileDialog.HasSelected())
            {
                Info(fmt::format("Selected {}", FileDialog.GetSelected().string()).c_str());
                FileDialog.ClearSelected();
            }
        }
        ImGui::End();
    }

    void Emulator::Run()
    {
        // TODO: stupid hack
        // Render.Index = 1;
        Render.Finalize();
        Input.Finalize();
        Audio.Finalize();

        Frame.Open();

        ImGui_ImplGlfw_InitForOpenGL(Frame, true);
        
        Render.Current()->Attach();

        while(!glfwWindowShouldClose(Frame))
        {
            auto* RenderBackend = Render.Current();
            RenderBackend->Begin();
            ImGui_ImplGlfw_NewFrame();

            ImGui::NewFrame();
            GUI();
            ImGui::Render();

            RenderBackend->End();
            glfwPollEvents();
        }

        Render.Current()->Detach();
        ImGui_ImplGlfw_Shutdown();

        Frame.Close();
    }
}