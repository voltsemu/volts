#include "Frame.h"

#include "Core/Logger/Logger.h"
#include "Render/Render.h"
#include "Render/Metal/Render.h"

#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_osx.h"
#include "imgui/examples/imgui_impl_metal.h"

#define WINDOW(W) ((NSWindow*)W)

// application that fixes some bugs in metal
@interface VApp : NSApplication
@end

@implementation VApp
- (void)sendEvent:(NSEvent*)event
{
    if([event type] == NSEventTypeKeyUp && ([event modifierFlags] & NSEventModifierFlagCommand))
        [[self keyWindow] sendEvent:event];
    else
        [super sendEvent:event];
}
@end




@interface VWindowDelegate : NSObject<NSWindowDelegate>
@end

@implementation VWindowDelegate

- (BOOL)windowShouldClose:(id)sender
{
    return YES;
}

@end




@interface VViewDelegate : NSObject<MTKViewDelegate>
{
    Volts::GUI::Frame* F;
}

@property(nonatomic, strong) id Queue;

@end

@implementation VViewDelegate

- (instancetype)initWithFrame:(Volts::GUI::Frame*)frame
{
    self = [super init];
    F = frame;
    return self;
}

- (void)mtkView:(nonnull MTKView*)view drawableSizeWillChange:(CGSize)size
{
}

- (void)drawInMTKView:(nonnull MTKView*)view
{
    ImGuiIO& IO = ImGui::GetIO();
    IO.DisplaySize.x = view.bounds.size.width;
    IO.DisplaySize.y = view.bounds.size.height;

    CGFloat BufferScale = view.window.screen.backingScaleFactor ?: NSScreen.mainScreen.backingScaleFactor;

    IO.DisplayFramebufferScale = ImVec2(BufferScale, BufferScale);

    IO.DeltaTime = float(1.f / 60.f);

    id<MTLCommandBuffer> Buffer = [self.Queue commandBuffer];

    MTLRenderPassDescriptor* RenderPass = view.currentRenderPassDescriptor;
    if(RenderPass != nil)
    {
        RenderPass.colorAttachments[0].clearColor = MTLClearColorMake(0.28f, 0.36f, 0.5f, 1.0f);

        id<MTLRenderCommandEncoder> Encoder = [Buffer renderCommandEncoderWithDescriptor:RenderPass];
        [Encoder pushDebugGroup:@"Imgui layer"];

        ImGui_ImplMetal_NewFrame(RenderPass);
        ImGui_ImplOSX_NewFrame(view);

        ImGui::NewFrame();

        // run the main UI
        Volts::GUI::Frame::Singleton->GUILoop();

        ImGui::Render();
        ImDrawData* Draw = ImGui::GetDrawData();
        ImGui_ImplMetal_RenderDrawData(Draw, Buffer, Encoder);

        [Encoder popDebugGroup];
        [Encoder endEncoding];

        [Buffer presentDrawable:view.currentDrawable];
    }

    [Buffer commit];
}
@end



@interface VMTKView : MTKView
@end

@implementation VMTKView

- (BOOL)isOpaque
{
    return YES;
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (void)mouseDown:(NSEvent*)event
{
    ImGui_ImplOSX_HandleEvent(event, self);
}

- (void)mouseUp:(NSEvent*)event
{
    ImGui_ImplOSX_HandleEvent(event, self);
}

- (void)mouseMoved:(NSEvent*)event
{
    ImGui_ImplOSX_HandleEvent(event, self);
}

- (void)mouseDragged:(NSEvent*)event
{
    ImGui_ImplOSX_HandleEvent(event, self);
}

- (void)scrollWheel:(NSEvent*)event
{
    ImGui_ImplOSX_HandleEvent(event, self);
}

- (void)keyDown:(NSEvent*)event
{
    ImGui_ImplOSX_HandleEvent(event, self);
}

- (void)keyUp:(NSEvent*)event
{
    ImGui_ImplOSX_HandleEvent(event, self);
}

- (void)flagsChanged:(NSEvent*)event
{
    ImGui_ImplOSX_HandleEvent(event, self);
}

@end




@interface VAppDelegate : NSObject<NSApplicationDelegate>
{
    NSWindow* Window;
    id WindowDelegate;
    VViewDelegate* ViewDelegate;
    id MetalDevice;
    VMTKView* MetalView;
    Volts::GUI::Frame* F;
}
@end

@implementation VAppDelegate

- (instancetype)initWithFrame:(Volts::GUI::Frame*)frame
{
    self = [super init];
    F = frame;
    return self;
}

// this is essentially main() for mac
- (void)applicationDidFinishLaunching:(NSNotification*)notify
{
    // create the delegate
    WindowDelegate = [[VWindowDelegate alloc] init];
    // crate the window
    Window = [
            [NSWindow alloc]
            initWithContentRect:NSMakeRect(0, 0, 500, 500)
                styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
                backing:NSBackingStoreBuffered
                defer:NO
    ];

    // move it around and decorate it
    [Window setTitle:@"Volts"];
    [Window setAcceptsMouseMovedEvents:YES];
    [Window center];
    [Window setRestorable:YES];
    // use the delegate
    [Window setDelegate:WindowDelegate];

    /// set the view delegate and metal device

    ViewDelegate = [[VViewDelegate alloc] initWithFrame:F];
    MetalDevice = MTLCreateSystemDefaultDevice();
    if(MetalDevice == nil)
    {
        NSAlert* Alert = [[NSAlert alloc] init];
        [Alert setMessageText:@"Metal device not found"];
        [Alert setInformativeText:@"This device does not support the metal API. Metal was introduced in OSX 10.11"];
        [Alert beginSheetModalForWindow:Window completionHandler:^(NSModalResponse resp) {
            [NSApp terminate:nil];
        }];

        return;
    }
    MetalView = [[VMTKView alloc] init];

    [MetalView setDelegate:ViewDelegate];
    [MetalView setDevice:MetalDevice];
    [MetalView setColorPixelFormat:MTLPixelFormatBGRA8Unorm];
    [MetalView setDepthStencilPixelFormat:MTLPixelFormatDepth32Float_Stencil8];

    [Window setContentView:MetalView];
    CGSize Space = { (CGFloat)500, (CGFloat)500 };
    [MetalView setDrawableSize:Space];
    [[MetalView layer] setMagnificationFilter:kCAFilterNearest];
    [Window makeKeyAndOrderFront:nil];

    ViewDelegate.Queue = [MetalDevice newCommandQueue];

    F->Handle = (__bridge void*)Window;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplMetal_Init(MetalDevice);
    ImGui_ImplOSX_Init();

    F->CurrentRender = new Volts::RSX::Metal();
}

- (void)applicationWillTerminate:(NSNotification*)notify
{
    ImGui_ImplMetal_Shutdown();
    ImGui_ImplOSX_Shutdown();
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)app
{
    return YES;
}
@end



namespace Volts::GUI
{
    Frame::Frame()
    {

    }

    Frame& Frame::SetTitle(const String& T) { return *this; }

    Size Frame::GetSize() const
    {
        CGSize S = [WINDOW(CFBridgingRelease(Handle)) contentView].frame.size;
        return { 
            static_cast<U32>(S.width), 
            static_cast<U32>(S.height)
        };
    }

    void Frame::Run()
    {
        Frame::Singleton->CurrentRender = new RSX::Metal();
        // run the application
        [VApp sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp setDelegate:[[VAppDelegate alloc] initWithFrame:this]];
        [NSApp activateIgnoringOtherApps:YES];
        [NSApp run];
    }
}