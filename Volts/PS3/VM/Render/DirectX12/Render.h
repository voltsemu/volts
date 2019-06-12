#pragma once

#include "PS3/VM/Render/Render.h"

namespace Volts::PS3::RSX
{
    struct DirectX12 : Render
    {
        virtual ~DirectX12() override {}

        virtual InitError Init() override;
        virtual void DeInit() override;
        virtual bool Supported() const override;

        virtual RenderDevice* Devices(unsigned& Count) const override;

        virtual const char* Name() const override { return "DirectX12"; }
        virtual const char* Detail() const override { return "DirectX12 is a windows only graphics API with similar performance to vulkan"; }
    };

    VSCRIPT({
        Render::Register(new DirectX12());
    });
}
