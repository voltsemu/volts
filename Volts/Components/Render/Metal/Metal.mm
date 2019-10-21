#include "Metal.h"

#import <simd/simd.h>

#include "imgui.h"
#include "imgui/examples/imgui_impl_metal.h"

namespace Volts::Render
{
    static Metal* Singleton = new Metal();
    Metal::Metal()
    {
        Emulator::Get()->Render.Register(this);

        NSArray<id<MTLDevice>>* MTLDevices = MTLCopyAllDevices();

        for(id<MTLDevice> Device in MTLDevices)
            DeviceList.push_back(Device);
    }

    const Device* Metal::Devices(U32* Count) const
    {
        *Count = DeviceList.size();
        return DeviceList.data();
    }

    void Metal::SetDevice(U32 Index)
    {
        if(Index != DeviceIndex)
        {
            DeviceIndex = Index;
            ImGui_ImplMetal_Shutdown();
            CreatePipeline();
        }
    }

    void Metal::CreatePipeline()
    {
        NSWindow* Win = glfwGetCocoaWindow(Emulator::Get()->Frame);
        Layer = [CAMetalLayer layer];
        Layer.device = CurrentDevice();
        Layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        Win.contentView.layer = Layer;
        Win.contentView.wantsLayer = YES;

        NSError* Error = nil;
        MTLCompileOptions* CompileOptions = [MTLCompileOptions new];
        CompileOptions.languageVersion = MTLLanguageVersion1_1;

        id<MTLLibrary> Library = [CurrentDevice() newLibraryWithSource:
            @(R"(
                #include <metal_stdlib>
                #include <simd/simd.h>

                using namespace metal;

                struct RasterData
                {
                    float4 Position [[position]];
                    float4 Colour;
                };

                struct Vertex
                {
                    vector_float2 Position;
                    vector_float4 Colour;
                };

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
            options:CompileOptions
            error:&Error
        ];

        if(!Library)
        {
            NSLog(@"Cant create library %@", Error);
            glfwTerminate();
            return;
        }

        id<MTLFunction> Vert = [Library newFunctionWithName:@"VertShader"];
        id<MTLFunction> Frag = [Library newFunctionWithName:@"FragShader"];

        Queue = [CurrentDevice() newCommandQueue];

        MTLRenderPipelineDescriptor* PipelineDesc = [MTLRenderPipelineDescriptor new];
        PipelineDesc.vertexFunction = Vert;
        PipelineDesc.fragmentFunction = Frag;
        PipelineDesc.colorAttachments[0].pixelFormat = Layer.pixelFormat;

        PipelineState = [CurrentDevice() newRenderPipelineStateWithDescriptor:PipelineDesc error:nil];
        ImGui_ImplMetal_Init(CurrentDevice());
    }

    void Metal::Attach()
    {
        CreatePipeline();
    }

    void Metal::Detach()
    {
        ImGui_ImplMetal_Shutdown();
    }

    void Metal::Begin()
    {
        U32 Width, Height;

        glfwGetFramebufferSize(Emulator::Get()->Frame, (I32*)&Width, (I32*)&Height);

        Layer.drawableSize = CGSizeMake(Width, Height);
        CurrentDrawable = [Layer nextDrawable];

        Buffer = [Queue commandBuffer];

        MTLRenderPassDescriptor* RenderPass = [MTLRenderPassDescriptor new];
        MTLRenderPassColorAttachmentDescriptor* Col = RenderPass.colorAttachments[0];
        Col.texture = CurrentDrawable.texture;
        Col.loadAction = MTLLoadActionClear;
        Col.clearColor = MTLClearColorMake(1.0, 1.0, 1.0, 1.0);
        Col.storeAction = MTLStoreActionStore;
        Encoder = [Buffer renderCommandEncoderWithDescriptor:RenderPass];

        [Encoder setViewport:(MTLViewport){ 0.f, 0.f, (double)Width, (double)Height, 0.f, 1.f }];
        [Encoder setRenderPipelineState:PipelineState];
        
        static const Vertex Verts[] = 
        {
            { { 250, -250 }, { 1, 0, 0, 1 } },
            { { -250, -250 }, { 0, 1, 0, 1 } },
            { { 0, 250 }, { 0, 0, 1, 1 } }
        };

        vector_uint2 FrameSize = { Width, Height };

        [Encoder setVertexBytes:Verts length:sizeof(Verts) atIndex:0];
        [Encoder setVertexBytes:&FrameSize length:sizeof(FrameSize) atIndex:1];
        [Encoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];

        ImGui_ImplMetal_NewFrame(RenderPass);
    }

    void Metal::End()
    {
        ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), Buffer, Encoder);
        [Encoder endEncoding];
        [Buffer presentDrawable:CurrentDrawable];
        [Buffer commit];
    }

    void Metal::Resize(U32 Width, U32 Height)
    {

    }

    void Metal::UpdateVSync(bool Enabled)
    {
        
    }
}