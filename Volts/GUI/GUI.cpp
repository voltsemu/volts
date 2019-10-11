#include "Frame.h"
#include "Render/Render.h"

#include "Config.h"

#include "Core/Logger.h"

#include "PS3/Util/Decrypt/UNSELF.h"

#include <chrono>
#include <thread>

#include "GUIExtensions.h"

#if CC_CLANG
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wformat-security"
#endif

namespace ImGui
{
    void HelpText(const char* Text)
    {
        ImGui::TextDisabled("(?)");
        if(ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35);
            ImGui::TextUnformatted(Text);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }
}

#if 0

namespace Volts::GUI
{
    bool ShowGPUOptions = true;
    void GPUOptions()
    {
        ImGui::Begin("GPU Options", &ShowGPUOptions, ImGuiWindowFlags_AlwaysAutoResize);

        /// Show renderer options, TODO: make this do stuff
        ImGui::Combo("Renderer", &Frame::Singleton->RenderIndex, Frame::Singleton->RenderNames, Frame::Singleton->RenderCount);

        if(!!Frame::Singleton->DeviceNames)
            ImGui::Combo("Device", &Frame::Singleton->DeviceIndex, Frame::Singleton->DeviceNames, Frame::Singleton->DeviceCount);

        static bool EnableVSync = false;
        ImGui::Checkbox("VSync", &EnableVSync);
        Frame::Singleton->CurrentRender->UpdateVSync(EnableVSync);

        Frame::Singleton->CurrentRender->Options();

        ImGui::End();
    }

    bool ShowCPUOptions = true;
    U32 CoreCount = std::thread::hardware_concurrency();
    void CPUOptions()
    {
        ImGui::Begin("CPU Options", &ShowCPUOptions);

        const char* SPURuntimeOptions[] = { "Interpreter", "JIT", "Recompiler" };
        static int CurrentSPURuntime = 0;
        ImGui::Combo("SPU Runtime", &CurrentSPURuntime, SPURuntimeOptions, IM_ARRAYSIZE(SPURuntimeOptions));

        const char* PPURuntimeOptions[] = { "Interpreter", "JIT", "Recompiler" };
        static int CurrentPPURuntime = 0;
        ImGui::Combo("PPU Runtime", &CurrentPPURuntime, PPURuntimeOptions, IM_ARRAYSIZE(PPURuntimeOptions));

        static int PPUThreads = 1;
        ImGui::SliderInt("PPU Threads", &PPUThreads, 1, CoreCount);

        ImGui::End();
    }

    bool ShowMetrics = true;
    void Metrics()
    {
        ImGui::Begin("Metrics", &ShowMetrics);

        TimePoint Now = std::chrono::high_resolution_clock::now();
        TimeDiff FrameTime = Now - Frame::Singleton->Metrics.LastFrame;
        ImGui::Text(fmt::format("Total FrameTime: {:.2f}ms", FrameTime.count()).c_str());
        ImGui::Text(fmt::format("FPS: {:.2f}", 1000 / FrameTime.count()).c_str());

        Frame::Singleton->Metrics.LastFrame = Now;
        ImGui::End();
    }

    bool ShowLogs = true;
    void Logs()
    {
        ImGui::Begin("Logs", &ShowLogs);

        const char* LevelOptions[] = { "Info", "Warn", "Error", "Fatal" };
        ImGui::Combo("Filter", (I32*)&Frame::Singleton->CurrentLevel, LevelOptions, IM_ARRAYSIZE(LevelOptions));

        ImGui::SameLine();
        if(ImGui::Button("Clear"))
            Frame::Singleton->LogBuffer.clear();

        ImGui::Separator();
        ImGui::TextUnformatted(Frame::Singleton->LogBuffer.c_str());

        ImGui::End();
    }

    bool ShowWindows = true;
    ImGui::FileBrowser UNSELFDialog;
    void Windows()
    {
        ImGui::Begin("Windows", &ShowWindows, ImGuiWindowFlags_MenuBar);

        if(ImGui::BeginMenuBar())
        {
            if(ImGui::BeginMenu("Settings"))
            {
                ImGui::MenuItem("GPU Settings", nullptr, &ShowGPUOptions);
                ImGui::MenuItem("CPU Settings", nullptr, &ShowCPUOptions);
                ImGui::MenuItem("Metrics", nullptr, &ShowMetrics);
                ImGui::MenuItem("Logs", nullptr, &ShowLogs);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        if(ImGui::Button("Do the thing"))
            UNSELFDialog.Open();

        ImGui::End();

        UNSELFDialog.Display();
        if(UNSELFDialog.HasSelected())
        {
            Cthulhu::FileSystem::BufferedFile F{UNSELFDialog.GetSelected().string().c_str()};
            PS3::UNSELF::DecryptSELF(F);
            UNSELFDialog.ClearSelected();
        }

        if(ShowGPUOptions) GPUOptions();
        if(ShowCPUOptions) CPUOptions();
        if(ShowMetrics) Metrics();
        if(ShowLogs) Logs();
    }

    void Frame::InitGUI()
    {
        UNSELFDialog.SetTitle("UNSELF");
    }

    void Frame::GUILoop()
    {
        if(ShowWindows) Windows();

#if GUIDEBUG
        if(static bool Demo = true; Demo) ImGui::ShowDemoWindow(&Demo);
#endif
    }
}

#endif

#if CC_CLANG
#   pragma clang diagnostic pop
#endif
