#pragma once

#include "Render/Render.h"

#include <vulkan/vulkan.h>

namespace Volts::RSX
{
    struct Vulkan : Render
    {
        virtual void Start(GUI::Frame* Handle) override;
        virtual void Detach() override;
        virtual const String& Name() const override { return "Vulkan"; }
        virtual const String& Description() const override { return "Vulkan3D"; }
    private:
        bool SetupVulkan();

        VkResult CreateInstance();
    };
}