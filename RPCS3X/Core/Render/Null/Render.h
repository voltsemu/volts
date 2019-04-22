#pragma once

#include "Core/Render/Render.h"

namespace RPCS3X::RSX
{
    struct NullRender : public Render
    {
        virtual ~NullRender() override {}
        virtual void Initialize() override {}
        virtual void Close() override {}
        virtual void Resize(U32 Width, U32 Height) override {}
        virtual void Minimize() override {}
        virtual void Maximize() override {}

        virtual void Pause() override {}
        virtual void Resume() override {}
        virtual RenderState SaveState() const override { return nullptr; }
        virtual void LoadFromState(RenderState State) override {}

        virtual Str Name() const override { return "Null"; }
        virtual Str Description() const override { return "Null renderer. No video output"; }

        virtual void Present() override {}

        virtual void ApplyUpscale(U32 Scale) override {}
        virtual U32 ResolutionScale() const override { return 0; }

        virtual void ApplyAnisotropic(U8 Level) override {}
        virtual U8 AnisotropicStrength() const override { return 0; }

        virtual void ApplyAntiAlias(U8 Level) override {}
        virtual U8 AliasStength() const override { return 0; }

        virtual void ApplyFramelimit(U16 Limit) override {}
        virtual U16 FrameLimit() const override { return 0; }

        virtual void ApplyVSync(bool Sync) override {}
        virtual bool UsingVSync() const override { return false; }

        virtual void SwitchDevice(GPUDeviceID ID) override {}
        virtual GPUDevices CompatibleDevices() const override { return GPUDevices(); }
        virtual void FreeDevices(GPUDevices Devices) const override {}

        virtual Str DeviceName(const GPUDeviceID& Device) const override { return ""; }
    };
}