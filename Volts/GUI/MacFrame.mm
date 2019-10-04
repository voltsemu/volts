#include "Frame.h"

#include "Core/Logger/Logger.h"
#include "Render/Render.h"
#include "Render/Metal/Render.h"

#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <QuartzCore/CAMetalLayer.h>

using Volts::GUI::Frame;

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

@interface VApp : NSApplication
@end

@implementation VApp

- (void)sendEvent:(NSEvent*) event
{
    if ([event type] == NSEventTypeKeyUp && ([event modifierFlags] & NSEventModifierFlagCommand))
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

- (void)windowWillClose:(NSNotification*)notify
{
    puts("Closed");
    Frame::Singleton->CurrentRender->Detach();
}

@end


@interface VAppDelegate : NSObject<NSApplicationDelegate>
@end

void RenderLoop(
    CVDisplayLinkRef Link,
    const CVTimeStamp* Now,
    const CVTimeStamp* OutTime,
    CVOptionFlags FlagsIn,
    CVOptionFlags* FlagsOut,
    void* Data
)
{
    auto* F = (Frame*)Data;
    F->CurrentRender->BeginRender();
    ImGui::NewFrame();
    
    F->GUILoop();
    
    F->CurrentRender->PresentRender();
}

@implementation VAppDelegate

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender 
{
    return YES;
}

// basically main on mac
- (void)applicationDidFinishLaunching:(NSNotification*)notify
{
    NSWindow* Window = [
        [NSWindow alloc]
        initWithContentRect:NSMakeRect(0, 0, 720, 480)
            styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
            backing:NSBackingStoreBuffered
            defer:NO
    ];

    [Window setTitle:[NSString stringWithUTF8String:Frame::Singleton->GetTitle()]];
    [Window setAcceptsMouseMovedEvents:YES];
    [Window center];
    [Window setRestorable:YES];
    [Window setDelegate:[VWindowDelegate new]];

    // this fixes ARC trying to cleanup the window memory after its already been cleaned up
    [Window setReleasedWhenClosed:NO];

    VMTKView* View = [VMTKView new];
    [View setDevice:(__bridge id<MTLDevice>)Frame::Singleton->CurrentRender->GetDevice()];
    [View setColorPixelFormat:MTLPixelFormatBGRA8Unorm];
    [View setDepthStencilPixelFormat:MTLPixelFormatDepth32Float_Stencil8];

    [Window setContentView:View];

    CVDisplayLinkCreateWithActiveCGDisplays((CVDisplayLinkRef*)&Frame::Singleton->DisplayLink);
    CVDisplayLinkSetOutputCallback(
        (CVDisplayLinkRef)Frame::Singleton->DisplayLink, 
        (CVDisplayLinkOutputCallback)&RenderLoop, 
        Frame::Singleton
    );
    CVDisplayLinkStart((CVDisplayLinkRef)Frame::Singleton->DisplayLink);

    Frame::Singleton->Window = (__bridge void*)Window;
    Frame::Singleton->Handle = (__bridge void*)Window.contentView.layer;

    Frame::Singleton->CurrentRender->Attach(Frame::Singleton);

    [Window makeKeyAndOrderFront:nil];
}

@end


#pragma mark Public interface

namespace Volts::GUI
{
    Size Frame::GetSize() const
    {
        CGSize S = [(__bridge NSWindow*)Handle contentView].frame.size;
        return {
            static_cast<U32>(S.width),
            static_cast<U32>(S.height)
        };
    }

    void Frame::Run()
    {
        FinalizeRenders();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        SetRender("Metal");
        UpdateDevices();

        [VApp sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp setDelegate:[VAppDelegate new]];
        [NSApp activateIgnoringOtherApps:YES];
        [NSApp run];
    }

    Frame::~Frame()
    {
        CVDisplayLinkRelease((CVDisplayLinkRef)DisplayLink);
        ImGui::DestroyContext();
    }
}
