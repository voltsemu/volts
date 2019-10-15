#pragma once

#include "Render/Render.h"

#import <Metal/Metal.h>

namespace Volts::Render
{
    struct MetalDevice : Device
    {
        virtual ~MetalDevice() {}
        virtual const char* Name() const override 
        {
            
        }

        id<MTLDevice> Handle;
    };
}