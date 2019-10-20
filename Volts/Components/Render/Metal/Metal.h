#pragma once

#import <QuartzCore/QuartzCore.h>
#include "Support.h"
#include <vector>
#include <simd/simd.h>

namespace Volts::Render
{    
    struct Vertex
    {
        vector_float2 Position;
        vector_float4 Colour;
    };

    struct Metal : Render
    {
        Metal();
        virtual ~Metal() override {}
        virtual const char* Name() const override { return "Metal"; }

        virtual const Device* Devices(U32* Count) const override;
        virtual void SetDevice(U32 Index) override;

        virtual void Attach() override;
        virtual void Detach() override;

        virtual void Begin() override;
        virtual void End() override;

        virtual void Resize(U32 Width, U32 Height) override;

    private:
        // api data
        U32 DeviceIndex = 0;
        std::vector<MetalDevice> DeviceList;
        id CurrentDevice() { return DeviceList[DeviceIndex].Handle; }

        // internal data
        id<MTLRenderPipelineState> PipelineState;
        id<MTLCommandQueue> Queue;
        id<MTLCommandBuffer> Buffer;
        id<MTLRenderCommandEncoder> Encoder;
        CAMetalLayer* Layer;
        id<CAMetalDrawable> CurrentDrawable;

        void CreatePipeline();
    };
}