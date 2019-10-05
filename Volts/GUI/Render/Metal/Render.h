#pragma once

#include "Support.h"

#include "Frame.h"

#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_osx.h"
#include "imgui/examples/imgui_impl_metal.h"

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
    struct Metal : Render
    {
        Metal();
        virtual ~Metal() override {}
        virtual void Attach(GUI::Frame* Frame) override;
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

        id<MTLDevice> CurrentDevice() const
        {
            return DeviceList[DeviceIndex].Handle;
        }
        GUI::Frame* Frame;
        MetalSupport::MetalDevice* DeviceList;
        U32 DeviceIndex = 0;
        U32 DeviceCount;

        VView* View;

        void CreatePipeline();
        id<MTLCommandQueue> CommandQueue;
        id<MTLCommandBuffer> CommandBuffer;
        id<MTLRenderCommandEncoder> Encoder;
        MTLRenderPassDescriptor* RenderPass;
    };
}