#pragma once

#include "rsx/rsx.h"

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

namespace volts::rsx::metal
{
    struct mtkdevice : device
    {
        mtkdevice(id h) : handle(h) {}
        virtual ~mtkdevice() override {}

        virtual const std::wstring& name() const override
        {
            NSData* data = [[(id<MTLDevice>)handle name] dataUsingEncoding:kCFStringEncodingUTF32LE];
            return std::wstring((wchar_t*)[data bytes], [data length] / sizeof(whcar_t));
        }

        id handle;
    }
}