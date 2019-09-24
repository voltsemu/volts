#pragma once

#include "Render/Render.h"

namespace Volts::MetalSupport
{
    struct MetalDevice : RSX::Device
    {
        MetalDevice() {}
        MetalDevice(void* Dev) : Handle(Dev) {}

        std::wstring Name() const override;
    private:
        void* Handle;
    };
}