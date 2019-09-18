#pragma once

#include "Render/Render.h"

#include <vulkan/vulkan.h>
#include "imgui/examples/imgui_impl_vulkan.h"

namespace Volts::RSX
{
    struct Vulkan : Render
    {
        virtual void Start(GUI::Frame* Handle) override;
        virtual void Detach() override;
        virtual const String& Name() const override { return "Vulkan"; }
        virtual const String& Description() const override { return "Vulkan3D"; }

        virtual void InitGUI() const override
        {
            // TODO: info
            ImGui_ImplVulkan_Init();
        }

        virtual void NewGUIFrame() const override
        {
            ImGui_ImplVulkan_NewFrame();
        }

        virtual void ShutdownGUI() const override
        {
            ImGui_ImplVulkan_Shutdown();
        }

        virtual void RenderGUI() const override
        {
            ImDrawData* Draw = ImGui::GetDrawData();
            ImGui_ImplVulkan_RenderDrawData(Draw, /* todo */);
        }

    private:
        bool Setup();

        bool CreateInstance();

        VkInstance Instance;
    };
}