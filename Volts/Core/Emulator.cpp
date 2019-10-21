#include "Emulator.h"

#include <iostream>
#include <filesystem>
#include <chrono>

#include "imgui.h"
#include "imgui/examples/imgui_impl_glfw.h"
#include "imgui/examples/imgui_impl_win32.h"
#include "imfilebrowser.h"

#include "Utils/SFO.h"
#include "Utils/UNSELF.h"

namespace Volts
{
    using namespace Utils;
    bool LogScrollToBottom = false;

    void Trace(const char* M)
    {
        auto* Emu = Emulator::Get();
        if(Emu->Level > LogLevel::Trace)
            return;

        Emu->LogBuffer.append(fmt::format("[trace] {}\n", M).c_str());
        LogScrollToBottom = true;
    }

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

    enum class DialogType : U8
    {
        None,
        PARAM,
        UNSELF,
    };

    using TimePoint = decltype(std::chrono::high_resolution_clock::now());
    using TimeDiff = std::chrono::duration<double, std::milli>;

    DialogType CurrentDialog;
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

            Render.Current()->UpdateVSync(VSyncEnabled);

            ImGui::Combo("Audio", &Audio.Index, Audio.Names, Audio.Count);
            ImGui::Combo("Input", &Input.Index, Input.Names, Input.Count);
        }
        ImGui::End();

        ImGui::Begin("Logs");
        {
            const char* LevelOptions[] = { "Trace", "Info", "Warn", "Error", "Fatal" };
            ImGui::Combo("Filter", (I32*)&Level, LevelOptions, IM_ARRAYSIZE(LevelOptions));

            ImGui::SameLine();
            if(ImGui::Button("Clear"))
                LogBuffer.clear();

            ImGui::Separator();
            ImGui::BeginChild("LogBox");

            ImGui::PushTextWrapPos();
            ImGui::TextUnformatted(LogBuffer.c_str());
            ImGui::PopTextWrapPos();

            if(LogScrollToBottom)
                ImGui::SetScrollHere(1.f);
            LogScrollToBottom = false;

            ImGui::EndChild();
        }
        ImGui::End();

        ImGui::Begin("Files");
        {
            if(ImGui::Button("Parse PARAM.SFO"))
            {
                CurrentDialog = DialogType::PARAM;
                FileDialog.SetTitle("SFO");
                FileDialog.Open();
            }

            if(ImGui::Button("Decrypt EBOOT"))
            {
                CurrentDialog = DialogType::UNSELF;
                FileDialog.SetTitle("UNSELF");
                FileDialog.Open();
            }

            FileDialog.Display();

            if(FileDialog.HasSelected())
            {
                // TODO: all this is debug code
                Info(fmt::format("Selected {}", FileDialog.GetSelected().string()).c_str());

                if(CurrentDialog == DialogType::PARAM)
                {
                    auto Obj = LoadSFO({FileDialog.GetSelected().string().c_str()});

                    for(auto& [Key, Val] : Obj)
                    {
                        std::string ValStr;
                        switch(Val.Type)
                        {
                            case Format::String:
                                ValStr = (char*)Val.Data.data();
                                break;
                            case Format::Array:
                                for(auto E : Val.Data)
                                    ValStr += std::to_string(E) + " ";
                                break;
                            case Format::Integer:
                                ValStr = std::to_string(*(U32*)Val.Data.data());
                                break;
                        }
                        VINFO("{}: {}", Key, ValStr);
                    }
                }
                else if(CurrentDialog == DialogType::UNSELF)
                {
                    auto Obj = LoadSELF({FileDialog.GetSelected().string().c_str()});

                    auto* F = fopen("VOLTS.BIN", "wb");
                    fwrite(Obj.GetData(), 1, Obj.Len(), F);
                    fclose(F);
                }
                FileDialog.ClearSelected();
            }
        }
        ImGui::End();
    }

    void Emulator::Run()
    {
        // TODO: stupid hack
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