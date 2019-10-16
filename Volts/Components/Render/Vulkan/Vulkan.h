#pragma once

#include "Support.h"

namespace Volts::Render
{
    struct Vulkan : Render
    {
        Vulkan();
        virtual ~Vulkan() override {}

        virtual const char* Name() const override { return "Vulkan"; }

        virtual const Device* Devices(U32* Count) const override;
        virtual void SetDevice(U32 Index) override;

        virtual void Begin() override;
        virtual void End() override;

        virtual void Attach() override;
        virtual void Detach() override;
    private:
        U32 DeviceIndex = 0;
        VkDevice CurrentDevice() const;
    };
}