#pragma once

#include "DX12Support.h"

#include <Core/Collections/Array.h>

namespace Volts::PS3::RSX
{
    struct DirectX12 : Render
    {
        DirectX12();
        virtual ~DirectX12() override {}

        virtual InitError Init() override;
        virtual void DeInit() override;
        virtual bool Supported() const override;

        virtual RenderDevice* Devices(unsigned& Count) const override;

        virtual const char* Name() const override { return "DirectX12"; }
        virtual const char* Detail() const override { return "DirectX12 is a windows only graphics API with similar performance to vulkan"; }

        virtual bool RequiresDevice() const override { return true; }

    private:
        bool Initialized = false;
        Cthulhu::U8 SwapFrames = 3;
        Cthulhu::Array<DX12::Device> RenderDevices;
    };
}
