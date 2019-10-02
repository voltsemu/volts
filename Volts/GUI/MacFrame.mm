#include "Frame.h"

#include "Core/Logger/Logger.h"
#include "Render/Render.h"
#include "Render/Metal/Render.h"

#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

#pragma mark Application delegate subclass

#pragma mark Application subclass

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

#pragma mark Public interface


namespace Volts::GUI
{
    Frame::Frame() {}
    
    Frame& Frame::SetTitle(const String& T) 
    { 
        Title = T;
        return *this;
    }

    String Frame::GetTitle() const 
    { 
        return Title;
    }

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
        [VApp sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp setDelegate:[VAppDelegate new]];
        [NSApp activateIgnoringOtherApps:YES];
        [NSApp run];
    }

}

#if 0

#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_osx.h"
#include "imgui/examples/imgui_impl_metal.h"

@interface VWindowDelegate : NSObject<NSWindowDelegate>
@end

@implementation VWindowDelegate

- (BOOL)windowShouldClose:(id)sender
{
    return YES;
}

@end


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



@interface VAppDelegate : NSObject<NSApplicationDelegate>
@end

@implementation VAppDelegate

- (void)applicationDidFinishLaunching:(NSNotification*)notify
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    NSWindow* Window = [
        [NSWindow alloc]
        initWithContentRect:NSMakeRect(0, 0, 500, 500)
            styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
            backing:NSBackingStoreBuffered
            defer:NO
    ];

    [Window setTitle:[NSString stringWithUTF8String:Volts::GUI::Frame::Singleton->GetTitle().CStr()]];
    [Window setAcceptsMouseMovedEvents:YES];
    [Window center];
    [Window setRestorable:YES];

    [Window setDelegate:[VWindowDelegate new]];

    Volts::GUI::Frame::Singleton->Handle = (__bridge void*)Window;

    Volts::GUI::Frame::Singleton->CurrentRender = new Volts::RSX::Metal();
    Volts::GUI::Frame::Singleton->CurrentRender->Attach(Volts::GUI::Frame::Singleton);

    [Window makeKeyAndOrderFront:nil];
}

- (void)applicationWillTerminate:(NSNotification*)notify
{
    Volts::GUI::Frame::Singleton->CurrentRender->Detach();
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)app
{
    return YES;
}

@end

namespace Volts::GUI
{
    Frame::Frame() {}

    Frame& Frame::SetTitle(const String& T) 
    { 
        Title = T;
        return *this; 
    }

    String Frame::GetTitle() const 
    { 
        return Title;
    }

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
        Frame::Singleton->CurrentRender = new RSX::Metal();
        // run the application
        [VApp sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp setDelegate:[[VAppDelegate alloc] init]];
        [NSApp activateIgnoringOtherApps:YES];
        [NSApp run];
    }
}

#endif