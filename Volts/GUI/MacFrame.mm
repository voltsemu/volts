#include "Frame.h"

#include "Core/Logger/Logger.h"
#include "Render/Render.h"
#include "Render/Metal/Render.h"

#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <QuartzCore/CAMetalLayer.h>

using Volts::GUI::Frame;

/**
the general interface flow we want is

- create window
- show window
- attach renderer
- begin loop
    - get input
    - begin render
    - do imgui
    - present render
    - repeat
- detach renderer
- close window
- shutdown
*/

@implementation VAppDelegate

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender
{
    return YES;
}

// basically main on mac
- (void)applicationDidFinishLaunching:(NSNotification*)notify
{
    // step 1: create window
    NSWindow* Window = [
        [NSWindow alloc]
        initWithContentRect:NSMakeRect(0, 0, 720, 480)
            styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
            backing:NSBackingStoreBuffered
            defer:NO
    ];

    // set basic display
    [Window setTitle:[NSString stringWithUTF8String:Frame::Singleton->GetTitle()]];
    [Window setAcceptsMouseMovedEvents:YES];
    [Window center];
    [Window setRestorable:YES];

    // this fixes ARC trying to cleanup the window memory after its already been cleaned up
    [Window setReleasedWhenClosed:NO];

    // step 2: show window
    [Window makeKeyAndOrderFront:nil];

    Frame::Singleton->Handle = (__bridge void*)Window;
    Frame::Singleton->CurrentRender->Attach(Frame::Singleton);

    while(bool StayOpen = true; StayOpen)
    {
        NSEvent* Event = [Window nextEventMatchingMask:NSEventMaskAny];
        ImGui_ImplOSX_HandleEvent(Event, [Window contentView]);

        Frame::Singleton->CurrentRender->BeginRender();
        ImGui_ImplOSX_NewFrame([Window contentView]);


        ImGui::NewFrame();
        Frame::Singleton->GUILoop();
        ImGui::Render();

        Frame::Singleton->CurrentRender->PresentRender();
    }

    Frame::Singleton->CurrentRender->Detach();
}

@end


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

        SetRender("Metal");
        UpdateDevices();

        [VApp sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp setDelegate:[VAppDelegate new]];
        [NSApp activateIgnoringOtherApps:YES];
        [NSApp run];
    }
}
