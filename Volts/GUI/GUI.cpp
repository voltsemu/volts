#include "Frame.h"
#include "Render/Render.h"
#include "imgui/imgui.h"

#include "Core/Logger/Logger.h"

#include "PS3/Util/Decrypt/UNSELF.h"

#include <chrono>

namespace Volts::GUI
{
    bool ShowGPUOptions = true;
    void GPUOptions()
    {
        ImGui::Begin("GPU Options", &ShowGPUOptions);

        /// Show renderer options, TODO: make this do stuff
        static int CurrentRenderer = 0;
        ImGui::Combo("Renderer", &CurrentRenderer, Frame::Singleton->RenderNames, Frame::Singleton->RenderCount);


        // this crashes for mysterious reasons
        static int CurrentDevice = 0;
        ImGui::Combo("Device", &CurrentDevice, Frame::Singleton->DeviceNames, Frame::Singleton->DeviceCount);

        static bool EnableVSync = false;
        ImGui::Checkbox("VSync", &EnableVSync);
        Frame::Singleton->CurrentRender->UpdateVSync(EnableVSync);

        ImGui::End();
    }

    bool ShowCPUOptions = true;
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
        ImGui::SliderInt("PPU Threads", &PPUThreads, 1, 4);

        ImGui::End();
    }

    using TimePoint = decltype(std::chrono::high_resolution_clock::now());
    TimePoint LastFrame;
    bool ShowMetrics = true;
    void Metrics()
    {
        ImGui::Begin("Metrics", &ShowMetrics);
        TimePoint Now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> FrameTime = Now - LastFrame;
        ImGui::Text(fmt::format("FrameTime: {:.2f}ms", FrameTime.count()).c_str());
        ImGui::Text(fmt::format("FPS: {:.2f}", 1000 / FrameTime.count()).c_str());

        LastFrame = Now;
        ImGui::End();
    }

    bool ShowWindows = true;
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
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::End();

        if(ShowGPUOptions) GPUOptions();
        if(ShowCPUOptions) CPUOptions();
        if(ShowMetrics) Metrics();
    }

    void Frame::GUILoop()
    {
        if(ShowWindows) Windows();

        if(static bool Demo = true; Demo) ImGui::ShowDemoWindow(&Demo);
    }
}