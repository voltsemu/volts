#pragma once

#import <QuartzCore/QuartzCore.h>
#include "Support.h"
#include <vector>

namespace Volts::Render
{
    struct Metal : Render
    {
        Metal();
        virtual ~Metal() {}
        virtual const char* Name() const override { return "Metal"; }

        virtual const Device* Devices(U32* Count) const override;
        virtual void SetDevice(U32 Index) override;

        virtual void Attach() override;
        virtual void Detach() override;

        virtual void Begin() override;
        virtual void End() override;

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