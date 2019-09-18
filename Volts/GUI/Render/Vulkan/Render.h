#pragma once

#include "Render/Render.h"

#include <vulkan/vulkan.h>
#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_vulkan.h"

namespace Volts::RSX
{
    struct Vulkan : Render
    {
        virtual ~Vulkan() {}
        virtual void Attach(GUI::Frame* Handle) override;
        virtual void Detach() override;
        virtual const String Name() const override { return "Vulkan"; }
        virtual const String Description() const override { return "Vulkan3D"; }

        virtual void BeginRender() override {}
        virtual void PresentRender() override {}
    };
}