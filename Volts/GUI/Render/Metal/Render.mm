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

        F32 VertexData[] = {
            0.f, 1.f, 0.f,
            -1.f, -1.f, 0.f,
            1.f, -1.f, 0.f
        };

        U32 DataSize = sizeof(VertexData) / sizeof(F32);
        VertexBuffer = [
            CurrentDevice() newBufferWithBytes:VertexData
                            length:DataSize
                            options:MTLResourceStorageModeManaged
        ];

        CreatePipelineState();
    }

    void Metal::Attach(GUI::Frame* F)
    {
        Frame = F;
        ImGui_ImplMetal_Init(CurrentDevice());
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
        auto Drawable = [((__bridge CAMetalLayer*)Frame->Handle) nextDrawable];
        MTLRenderPassDescriptor* RenderPass = [MTLRenderPassDescriptor renderPassDescriptor];
        RenderPass.colorAttachments[0].texture = Drawable.texture;
        RenderPass.colorAttachments[0].loadAction = MTLLoadActionClear;
        RenderPass.colorAttachments[0].clearColor = MTLClearColorMake(0.28f, 0.36f, 0.5f, 1.f);

        CommandBuffer = [CommandQueue commandBuffer];
        Encoder = [CommandBuffer renderCommandEncoderWithDescriptor:RenderPass];

        ImGui_ImplMetal_NewFrame(RenderPass);
        ImGui_ImplOSX_NewFrame([(__bridge NSWindow*)Frame->Window contentView]);
    }

    void Metal::PresentRender()
    {
        ImGui::Render();
        ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), CommandBuffer, Encoder);

        [Encoder endEncoding];
        [CommandBuffer presentDrawable:((MTKView*)[(__bridge NSWindow*)Frame->Window contentView]).currentDrawable];
        [CommandBuffer commit];
    }

    const char* ShaderSource = R"(
        vertex float4 VertexShader(
    const device packed_float3* VertexArray [[ buffer(0) ]],
    unsigned int ID [[ vertex_id ]]
)
{
    return float4(VertexArray[ID], 1.f);
}

fragment half4 FragmentShader()
{
    return half4(1.f);
}
    )";

    void Metal::CreatePipelineState()
    {
        auto* Library = [
            CurrentDevice() newLibraryWithSource:@(ShaderSource)
                            options:[MTLCompileOptions new] 
                            error:nil
        ];

        auto* VertShader = [Library newFunctionWithName:@"VertexShader"];
        auto* FragShader = [Library newFunctionWithName:@"FragmentShader"];

        MTLRenderPipelineDescriptor* PipelineStateDescriptor = [MTLRenderPipelineDescriptor new];
        PipelineStateDescriptor.vertexFunction = VertShader;
        PipelineStateDescriptor.fragmentFunction = FragShader;
        PipelineStateDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;

        PipelineState = [CurrentDevice() newRenderPipelineStateWithDescriptor:PipelineStateDescriptor error:nil];

        CommandQueue = [CurrentDevice() newCommandQueue];
    }
}