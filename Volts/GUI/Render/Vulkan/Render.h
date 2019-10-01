#pragma once

#include "Render/Render.h"

#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_vulkan.h"

#include "Support.h"

namespace Volts::RSX
{
    struct Vulkan : Render
    {
        Vulkan();
        virtual ~Vulkan();
        virtual void Attach(GUI::Frame* Handle) override;
        virtual void Detach() override;
        virtual const char* Name() const override { return "Vulkan"; }
        virtual const char* Description() const override { return "Vulkan3D"; }
        virtual Device* Devices(U32* Count) override;
        virtual void UpdateVSync(bool NewMode) override;

        virtual void BeginRender() override;
        virtual void PresentRender() override;

        virtual void Resize(GUI::Size NewSize) override;

        virtual void Windowed() override;
        virtual void Borderless() override;
        virtual void Fullscreen() override;
    private:
        GUI::Frame* Frame;
    };
}