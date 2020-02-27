#include "backend.h"

#include "render.h"

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>
#import <simd/simd.h>

#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_glfw.h"
#include "imgui/examples/imgui_impl_metal.h"

namespace volts::rsx
{
    struct mtl : render
    {
        virtual ~mtl() override {}

        virtual void preinit(const std::string& name) override
        {
            device = MTLCreateSystemDefaultDevice();

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        }

        virtual void postinit() override
        {
            ImGui_ImplMetal_Init(device);
            // TODO: at some point imgui will be getting a metal glfw backend
            ImGui_ImplGlfw_InitForOpenGL(window(), true);

            NSWindow* win = glfwGetCocoaWindow(window());

            layer = [CAMetalLayer layer];
            layer.device = device;
            layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
            
            win.contentView.layer = layer;
            win.contentView.wantsLayer = YES;
            
            MTLCompileOptions* options = [MTLCompileOptions new];
            options.languageVersion = MTLLanguageVersion1_1;

            queue = [device newCommandQueue];

            NSError* err = nil;

            id<MTLLibrary> library = [device newLibraryWithSource:
                @""
                "#include <metal_stdlib>\n"
                "using namespace metal;\n"
                "vertex float4 vs_main(constant float4* in [[buffer(0)]], uint id [[vertex_id]])\n"
                "{\n"
                "   return in[id];\n"
                "}\n"
                "fragment float4 frag_main(float4 in [[stage_in]])\n"
                "{\n"
                "   return float4(1, 0, 0, 1);\n"
                "}\n"
                options:options
                error:&err
            ];

            if(err != nil)
            {
                NSLog(@"%@", [err localizedDescription]);
                return;
            }

            MTLRenderPipelineDescriptor* desc = [MTLRenderPipelineDescriptor new];
            desc.vertexFunction = [library newFunctionWithName:@"vs_main"];
            desc.fragmentFunction = [library newFunctionWithName:@"frag_main"];
            desc.colorAttachments[0].pixelFormat = layer.pixelFormat;
            pipeline = [device newRenderPipelineStateWithDescriptor:desc error:nil];
        }

        virtual void begin() override
        {
            // TODO: extract this to resize
            glfwGetFramebufferSize(window(), &width, &height);
            layer.drawableSize = CGSizeMake(width, height);

            drawable = [layer nextDrawable];
            buffer = [queue commandBuffer];

            MTLRenderPassDescriptor* pass = [MTLRenderPassDescriptor new];
            MTLRenderPassColorAttachmentDescriptor* col = pass.colorAttachments[0];
            col.texture = drawable.texture;
            col.loadAction = MTLLoadActionClear;
            col.clearColor = MTLClearColorMake(0.45f, 0.55f, 0.60f, 1.f);
            col.storeAction = MTLStoreActionStore;

            encoder = [buffer renderCommandEncoderWithDescriptor:pass];
            [encoder setRenderPipelineState:pipeline];

            ImGui_ImplMetal_NewFrame(pass);
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }

        virtual void end() override
        {
            ImGui::Render();
            ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), buffer, encoder);

            [encoder endEncoding];
            [buffer presentDrawable:drawable];
            [buffer commit];
        }

        virtual void cleanup() override
        {
            ImGui_ImplMetal_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }

        virtual std::string_view name() const override { return "metal"; }

    private:
        
        // global data
        id<MTLDevice> device;
        CAMetalLayer* layer;
        id<MTLCommandQueue> queue;
        id<MTLRenderPipelineState> pipeline;

        int width;
        int height;

        // frame data
        id<CAMetalDrawable> drawable;
        id<MTLCommandBuffer> buffer;
        id<MTLRenderCommandEncoder> encoder;
    };

    void metal::connect()
    {
        rsx::add(new mtl());
    }
}