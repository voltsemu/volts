#pragma once

#include "Render/Render.h"

namespace Volts::MetalSupport
{
    struct MetalDevice : RSX::Device
    {
        MetalDevice() {}
        MetalDevice(id Dev) : Handle(Dev) {}
        virtual ~MetalDevice() {}

        virtual const char* Name() const override;
    
        id Handle;
    };
}