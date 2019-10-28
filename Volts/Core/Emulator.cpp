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

#include "ui.h"

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
        if(!ImGui::Begin("Volts"))
        {
            ImGui::End();
        }
        else
        {
            if(ImGui::CollapsingHeader("Metrics"))
            {
                auto Now = std::chrono::high_resolution_clock::now();

                TimeDiff Count = (Now - LastFrame);

                ImGui::Text("%s", fmt::format("FrameTime: {:.2f}ms", Count.count()).c_str());
                ImGui::Text("%s", fmt::format("FPS: {:.2f}", 1000 / Count.count()).c_str());

                LastFrame = Now;
            }

            if(ImGui::CollapsingHeader("Settings"))
            {
                ImGui::Combo("Renders", &Render.Index, Render.Names, Render.Count);


                if(ImGui::Combo("Device", &CurrentDevice, DeviceNames, DeviceCount))
                {
                    Render.Current()->SetDevice(CurrentDevice);
                }

                Render.Current()->GUI();

                ImGui::Separator();

                ImGui::Combo("Audio", &Audio.Index, Audio.Names, Audio.Count);
                ImGui::Combo("Input", &Input.Index, Input.Names, Input.Count);
            }

            if(ImGui::CollapsingHeader("Logs"))
            {
                const char* LevelOptions[] = { "Trace", "Info", "Warn", "Error", "Fatal" };
                if(ImGui::Combo("Filter", (I32*)&Level, LevelOptions, IM_ARRAYSIZE(LevelOptions)))
                {

                }

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

            if(ImGui::CollapsingHeader("Files"))
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
    }

    void Emulator::UpdateDeviceNames()
    {
        if(DeviceNames)
        {
            delete[] DeviceNames;
            DeviceNames = nullptr;
        }

        auto* Devices = Render.Current()->Devices(&DeviceCount);
        if(!Devices)
            return;

        DeviceNames = new char*[DeviceCount];
        for(U32 I = 0; I < DeviceCount; I++)
            DeviceNames[I] = strdup(Devices[I].Name());
    }

    uiControl* MakeGamesList()
    {
        uiBox* VBox = uiNewVerticalBox();

        return uiControl(VBox);
    }

    uiControl* MakeSettingsPage()
    {
        uiBox* HBox = uiNewHorizontalBox();
        uiBoxSetPadded(HBox, 1);

        auto* Group = uiNewGroup("Renderers");
        uiGroupSetMargined(Group, 1);
        uiBoxAppend(HBox, uiControl(Group), 1);

        auto* VBox = uiNewVerticalBox();
        uiBoxSetPadded(VBox, 1);
        uiGroupSetChild(Group, uiControl(VBox));

        {
            auto* RenderBox = uiNewCombobox();
            for(auto* Each : Emulator::Get()->Render.BackendList)
                uiComboboxAppend(RenderBox, Each->Name());
            uiBoxAppend(VBox, uiControl(RenderBox), 0);
        }

        {
            auto* ScalingSlider = uiNewSlider(100, 200);

            uiSliderOnChanged(ScalingSlider, [](auto* Slider, void*){
                uiSliderSetValue(Slider, uiSliderValue(Slider));
            }, nullptr);

            uiBoxAppend(VBox, uiControl(ScalingSlider), 0);
        }

        {
            auto* AudioBox = uiNewCombobox();
            for(auto* Each : Emulator::Get()->Audio.BackendList)
                uiComboboxAppend(AudioBox, Each->Name());
            uiBoxAppend(VBox, uiControl(AudioBox), 0);
        }

        {
            auto* InputBox = uiNewCombobox();
            for(auto* Each : Emulator::Get()->Input.BackendList)
                uiComboboxAppend(InputBox, Each->Name());
            uiBoxAppend(VBox, uiControl(InputBox), 0);
        }

        return uiControl(HBox);
    }

    void Emulator::Run()
    {
        //auto& IO = ImGui::GetIO();
        //IO.IniFilename = "Config/imgui.ini";

        Render.Finalize();
        Input.Finalize();
        Audio.Finalize();

        uiInitOptions Opts = {};
        uiInit(&Opts);

        auto* W = uiNewWindow("Volts", 720, 480, 1);

        uiWindowOnClosing(W, [](uiWindow*, void*){
            uiQuit();
            return 1;
        }, nullptr);

        uiOnShouldQuit([](void* Data){
            uiControlDestroy(uiControl(uiWindow(Data)));
            return 1;
        }, W);

        auto* Tab = uiNewTab();
        uiWindowSetChild(W, uiControl(Tab));
        uiWindowSetMargined(W, 1);

        uiTabAppend(Tab, "Games", MakeGamesList());
        uiTabSetMargined(Tab, 0, 1);

        uiTabAppend(Tab, "Settings", MakeSettingsPage());
        uiTabSetMargined(Tab, 1, 1);

        uiControlShow(uiControl(W));
        uiMain();
    }
}