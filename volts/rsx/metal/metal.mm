#include "rsx/rsx.h"

#include <spdlog/spdlog.h>

#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "gui/window.h"

#import <Metal/Metal.h>

#include "imgui/examples/imgui_impl_metal.h"

#include "support.h"

#import <simd/simd.h>

namespace volts::rsx
{
    struct vertex
    {
        vector_float2 position;
        vector_float4 colour;
    };

    struct metal : render
    {
        metal() 
        {
            rsx::add(this);

            NSArray<id<MTLDevice>>* nsdevices = MTLCopyAllDevices();

            for(id<MTLDevice> device in nsdevices)
                device_list.push_back(mtkdevice(device));
        }

        virtual ~metal() override {}

        virtual void attach() override
        {
            create_pipeline();
        }

        virtual void detach() override
        {
            ImGui_ImplMetal_Shutdown();
        }

        virtual void begin() override
        {
            int w, h;
            glfwGetFramebufferSize(gui::handle(), &w, &h);

            layer.drawableSize = CGMakeSize(w, h);
            drawable = [layer nextDrawable];

            buffer = [queue commandBuffer];

            MTLRenderPassDescriptor* pass = [MTLRenderPassDescriptor new]; 
            MTLRenderPassColorAttachmentDescriptor* colour = RenderPass.colorAttachments[0]; 

            colour.texture = drawable.texture;
            colour.loadAction = MTLLoadActionClear;
            colour.clearColor = MTLClearColorMake(1.0, 1.0, 1.0, 1.0);
            colour.storeAction = MTLStoreActionStore;
            encoder = [buffer renderCommandEncoderWithDescriptor:pass];

            [encoder setViewport:(MTLViewport){ 0.f, 0.f, (double)w, (double)h, 0.f, 1.f }];
            [encoder setRenderPipelineState:pipeline]; 

            const vertex verts[] = {
                { { 250, -250 }, { 1, 0, 0, 1 } },
                { { -250, -250 }, { 0, 1, 0, 1 } },
                { { 0, 250 }, { 0, 0, 1, 1 } }
            };

            vector_uint2 frame_size = { w, h };
            
            [encoder setVertexBytes:verts length:sizeof(verts) atIndex:0];
            [encoder setVertexBytes:&frame_size length:sizeof(frame_size) atIndex:1];
            [encoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];

            ImGui_ImplMetal_NewFrame(pass);
        }

        virtual void end() override
        {
            ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), buffer, encoder);
            [encoder endEncoding];
            [buffer presentDrawable:drawable];
            [buffer commit];
        }

        virtual std::string name() const override { return "Metal"; }

        virtual rsx::device* devices(int* len) const override
        {
            *len = device_list.size();
            return device_list.data();
        }

        void create_pipeline()
        {
            NSWindow* window = glfwGetCocoaWindow(gui::handle());
            layer = [CAMetalLayer new];
            layer.device = current_device();
            layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
            window.contentView.layer = layer;
            window.wantsContentView.wantsLayer = YES;

            NSError* err = nil;
            MTLCompileOptions* options = [MTLCompileOptions new];
            options.languageVersion = MTLLanguageVersion1_1;

            id<MTLLibrary> library = [current_device() newLibraryWithSource:
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
                options:options
                error:&err
            ];

            if(!library)
            {
                spdlog::error("failed to compile library");
                return;
            }

            id<MTLFunction> vert = [library newFunctionWithName:@"VertShader"];
            id<MTLFunction> frag = [library newFunctionWithName:@"FragShader"];

            queue = [current_device() newCommandQueue];

            MTLRenderPipelineDescriptor* desc = [MTLRenderPipelineDescriptor new];
            desc.vertexFunction = vert;
            desc.fragmentFunction = frag;
            desc.colorAttachments[0].pixelFormat = layer.pixelFormat;

            pipeline = [current_device() newRenderPipelineStateWithDescriptor:desc error:nil];
            ImGui_ImplMetal_Init(current_device());
        }

        int device_idx = 0;
        std::vector<mtkdevice> device_list;

        id<MTLDevice> current_device() const { return device_list[device_idx].handle; }

        id<MTLRenderPipelineState> pipeline;
        id<MTLCommandQueue> queue;
        id<MTLCommandBuffer> buffer;
        id<MTLRenderCommandEncoder> encoder;
        CAMetalLayer* layer;
        id<CAMetalDrawable> drawable;
    };
}