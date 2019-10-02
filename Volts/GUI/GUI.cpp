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
    bool ShowGPUOptions = true;
    void GPUOptions()
    {
        ImGui::Begin("GPU Options", &ShowGPUOptions);

        /// Show renderer options, TODO: make this do stuff
        const char* RenderOptions[] = { "Vulkan", "Metal", "OpenGL", "DirectX12", "Null" };
        static int CurrentRenderer = 0;
        ImGui::Combo("Renderer", &CurrentRenderer, RenderOptions, IM_ARRAYSIZE(RenderOptions));

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

    double LastFrame = 0;
    bool ShowMetrics = true;
    void Metrics()
    {
        ImGui::Begin("Metrics", &ShowMetrics);


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
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::End();

        if(ShowGPUOptions) GPUOptions();
        if(ShowCPUOptions) CPUOptions();
    }

    void Frame::GUILoop()
    {
        if(ShowWindows) Windows();

        if(static bool Demo = true; Demo) ImGui::ShowDemoWindow(&Demo);
    }
}