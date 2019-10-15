#pragma once

#include "Core/Emulator.h"

#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>

namespace Volts::Render
{
    struct MetalDevice : Device
    {
        MetalDevice() {}
        MetalDevice(id Dev) : Handle(Dev) {}
        virtual ~MetalDevice() {}

        virtual const char* Name() const override { return [(id<MTLDevice>)Handle name].UTF8String; }
    
        id Handle;
    };
}