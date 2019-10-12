#pragma once

#include "Support.h"

#include "Frame.h"

#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_osx.h"
#include "imgui/examples/imgui_impl_metal.h"

#include <simd/simd.h>

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

@class MTLBuffer;
@class MTLRenderPipelineState;

OBJC_CLASS(VView, MTKView)

- (BOOL)isOpaque
{
    return YES;
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

@end

namespace Volts::RSX
{
    typedef struct
    {
        vector_float2 Position;
        vector_float4 Colour;
    } Vertex;

    struct Metal : Render
    {
        Metal();
        virtual ~Metal() override {}
        virtual void Attach() override;
        virtual void Detach() override;

        virtual const char* Name() const override { return "Metal"; }
        virtual const char* Description() const override { return "MetalKit"; }

        virtual Device* Devices(U32* Count) override;
        virtual void SetDevice(RSX::Device* Device) override;
        virtual void UpdateVSync(bool Enable) override;

        virtual void Windowed() override;
        virtual void Fullscreen() override;
        virtual void Borderless() override;

        virtual void Resize(GUI::Size NewSize) override;

        virtual void BeginRender() override;
        virtual void PresentRender() override;
    private:

        id<MTLDevice> CurrentDevice() const { return DeviceList[DeviceIndex].Handle; }
        MetalSupport::MetalDevice* DeviceList;
        U32 DeviceIndex = 0;
        U32 DeviceCount;

        VView* View;

        void CreatePipeline();

        // persistent data
        id<MTLRenderPipelineState> PipelineState;
        id<MTLCommandQueue> CommandQueue;


        // frame data
        id<MTLCommandBuffer> CommandBuffer;
        id<MTLRenderCommandEncoder> Encoder;
        MTLRenderPassDescriptor* RenderPass;
    };
}