#include "Render.h"

namespace Volts::RSX
{
    static Metal* Singleton = new Metal();
    Metal::Metal()
    {
        Emulator::Get()->Register(this);

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

        GUI::Size S = Frame->GetSize();
        [Encoder setViewport:(MTLViewport){0.f, 0.f, (double)S.Width, (double)S.Height, 0.f, 1.f}];
        [Encoder setRenderPipelineState:PipelineState];

        static const Vertex Verts[] =
        {
            { { 250, -250 }, { 1, 0, 0, 1 } },
            { { -250, -250 }, { 0, 1, 0, 1 } },
            { { 0, 250 }, { 0, 0, 1, 1 } }
        };

        vector_uint2 FrameSize = { S.Width, S.Height };

        [Encoder setVertexBytes:Verts length:sizeof(Verts) atIndex:0];
        [Encoder setVertexBytes:&FrameSize length:sizeof(FrameSize) atIndex:1];
        [Encoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];

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
        id<MTLDevice> Dev = CurrentDevice();

        NSError* Error = nil;
        id<MTLLibrary> Lib = [Dev newLibraryWithSource:@(R"(
            #include <metal_stdlib>
            #include <simd/simd.h>

            using namespace metal;

            typedef struct {
                float4 Position [[position]];
                float4 Colour;
            } RasterData;

            typedef struct {
                vector_float2 Position;
                vector_float4 Colour;
            } Vertex;

            vertex RasterData VertShader(
                uint ID [[vertex_id]],
                constant Vertex* Verts [[buffer(0)]],
                constant vector_uint2* Size [[buffer(1)]]
            )
            {
                RasterData Out;

                float2 PixelSpacePosition = Verts[ID].Position.xy;

                Out.Position = vector_float4(0.f, 0.f, 0.f, 1.f);
                Out.Position.xy = PixelSpacePosition / (vector_float2(*Size) / 2.f);

                Out.Colour = Verts[ID].Colour;

                return Out;
            }

            fragment float4 FragShader(RasterData Data [[stage_in]])
            {
                return Data.Colour;
            }
        )")
                            options:[MTLCompileOptions new]
                            error:&Error];

        if(Lib == nil)
        {
            NSLog(@"Error %@", [Error localizedDescription]);
        }

        MTLRenderPipelineDescriptor* PipelineDescriptor = [MTLRenderPipelineDescriptor new];
        PipelineDescriptor.vertexFunction = [Lib newFunctionWithName:@"VertShader"];
        PipelineDescriptor.fragmentFunction = [Lib newFunctionWithName:@"FragShader"];
        PipelineDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
        PipelineState = [Dev newRenderPipelineStateWithDescriptor:PipelineDescriptor error:&Error];

        CommandQueue = [Dev newCommandQueue];
    }
}