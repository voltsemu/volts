#include "Frame.h"

#include "Core/Logger.h"
#include "Render/Render.h"

#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_osx.h"

#import <Cocoa/Cocoa.h>

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

OBJC_CLASS(VApp, NSApplication)
- (void)sendEvent:(NSEvent*)event
{
    if([event type] == NSEventTypeKeyUp && ([event modifierFlags] & NSEventModifierFlagCommand))
        [[self keyWindow] sendEvent:event];
    else
        [super sendEvent:event];
}
@end

OBJC_CLASS(VAppDelegate, NSObject<NSApplicationDelegate>)

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender
{
    return YES;
}

// basically main on mac
- (void)applicationDidFinishLaunching:(NSNotification*)notify
{
    Frame::Singleton->PreInit();
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

    // step 3: attach renderer
    Frame::Singleton->Handle = (__bridge void*)Window;
    Frame::Singleton->PostInit();

    ImGui_ImplOSX_Init();

    // step 4: main loop
    bool StayOpen = true;
    NSEvent* Event = nil;

    while(StayOpen)
    {
        // get input
        Event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                    untilDate:nil
                                    inMode:NSDefaultRunLoopMode
                                    dequeue:YES];
        if(Event)
        {
            [NSApp sendEvent:Event];
            ImGui_ImplOSX_HandleEvent(Event, [Window contentView]);
        }

        // begin render
        Frame::Singleton->CurrentRender->BeginRender();
        ImGui_ImplOSX_NewFrame([Window contentView]);

        // do imgui
        ImGui::NewFrame();
        Frame::Singleton->GUILoop();
        ImGui::Render();

        // present render
        Frame::Singleton->CurrentRender->PresentRender();
    }

    // detach
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

        [VApp sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp setDelegate:[VAppDelegate new]];
        [NSApp activateIgnoringOtherApps:YES];
        [NSApp run];
    }
}
