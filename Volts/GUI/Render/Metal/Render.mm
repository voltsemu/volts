#include "Render.h"

namespace Volts::RSX
{
    static Metal* Singleton = new Metal();
    Metal::Metal()
    {
        GUI::Frame::GetSingleton()->AddRender(this);

        NSArray<id<MTLDevice>>* MTLDevices = MTLCopyAllDevices();
        DeviceCount = [MTLDevices count];

        DeviceList = new MetalSupport::MetalDevice[DeviceCount];

        for(U32 I = 0; I < DeviceCount; I++)
            DeviceList[I] = MetalSupport::MetalDevice(MTLDevices[I]);

        // TODO
    }

    void Metal::Attach(GUI::Frame* F)
    {
        Frame = F;

        View = [VView new];
        [View setDevice:CurrentDevice()];
        [View setColorPixelFormat:MTLPixelFormatBGRA8Unorm];
        [View setDepthStencilPixelFormat:MTLPixelFormatDepth32Float_Stencil8];
        [[View layer] setMagnificationFilter:kCAFilterNearest];

        [(__bridge NSWindow*)Frame->Handle setContentView:View];
        ImGui_ImplMetal_Init(CurrentDevice());

        CreatePipeline();
    }

    void Metal::Detach()
    {
        ImGui_ImplMetal_Shutdown();
    }

    Device* Metal::Devices(U32* Count)
    {
        *Count = DeviceCount;
        return DeviceList;
    }

    void Metal::SetDevice(RSX::Device* Device)
    {
        for(U32 I = 0; I < DeviceCount; I++)
        {
            if(strcmp(DeviceList[I].Name(), Device->Name()) == 0)
            {
                DeviceIndex = I;
                break;
            }
        }
    }

    void Metal::UpdateVSync(bool NewMode)
    {
        // TODO: figure out vsync for metal
    }

    void Metal::Windowed()
    {

    }

    void Metal::Fullscreen()
    {

    }

    void Metal::Borderless()
    {

    }

    void Metal::Resize(GUI::Size NewSize)
    {

    }

    void Metal::BeginRender()
    {
        ImGuiIO& IO = ImGui::GetIO();
        MTKView* CurrentView = (MTKView*)[(__bridge NSWindow*)Frame->Handle contentView];

        IO.DisplaySize.x = CurrentView.bounds.size.width;
        IO.DisplaySize.y = CurrentView.bounds.size.height;

        CGFloat BufferScale = CurrentView.window.screen.backingScaleFactor ?: NSScreen.mainScreen.backingScaleFactor;

        IO.DisplayFramebufferScale = ImVec2(BufferScale, BufferScale);

        CommandBuffer = [CommandQueue commandBuffer];
        RenderPass = CurrentView.currentRenderPassDescriptor;
        RenderPass.colorAttachments[0].clearColor = MTLClearColorMake(0.28f, 0.36f, 0.5f, 1.0f);

        Encoder = [CommandBuffer renderCommandEncoderWithDescriptor:RenderPass];

        [Encoder pushDebugGroup:@"Imgui"];

        ImGui_ImplMetal_NewFrame(RenderPass);
    }

    void Metal::PresentRender()
    {
        ImDrawData* DrawData = ImGui::GetDrawData();
        ImGui_ImplMetal_RenderDrawData(DrawData, CommandBuffer, Encoder);
        
        [Encoder popDebugGroup];
        [Encoder endEncoding];

        MTKView* CurrentView = (MTKView*)[(__bridge NSWindow*)Frame->Handle contentView];
        [CommandBuffer presentDrawable:CurrentView.currentDrawable];

        [CommandBuffer commit];
    }

    void Metal::CreatePipeline()
    {
        CommandQueue = [CurrentDevice() newCommandQueue];
    }
}