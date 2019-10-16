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
                using namespace metal;

                vertex float4 VertShader(
                    constant float4* In [[buffer(0)]],
                    uint ID [[vertex_id]]
                )
                {
                    return In[ID];
                }

                fragment float4 FragShader(float4 In [[stage_in]])
                {
                    return float4(1, 0, 0, 1);
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
        float Ratio;
        I32 Width, Height;

        glfwGetFramebufferSize(Emulator::Get()->Frame, &Width, &Height);

        Ratio = float(Width / Height);

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

        [Encoder setRenderPipelineState:PipelineState];
        [Encoder setVertexBytes:(vector_float4[]){
            { 0, 0, 0, 1 },
            { -1, 1, 0, 1 },
            { 1, 1, 0, 1 }
        } length:3 * sizeof(vector_float4) atIndex:0];
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
}