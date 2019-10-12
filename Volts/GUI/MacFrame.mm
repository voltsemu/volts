#include "Frame.h"

#include "Core/Emulator.h"

#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_osx.h"

#import <Cocoa/Cocoa.h>

using Volts::GUI::Frame;
using namespace Volts;

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

- (void)applicationDidFinishLaunching:(NSNotification*)notify
{
    NSWindow* Window = [
        [NSWindow alloc]
        initWithContentRect:NSMakeRect(0, 0, 720, 480)
            styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
            backing:NSBackingStoreBuffered
            defer:NO
    ];

    auto& Emu = Emulator::Get();

    [Window setTitle:[NSString stringWithUTF8String:Emu.Window->Title]];
    [Window setAcceptsMouseMovedEvents:YES];
    [Window center];
    [Window setRestorable:YES];

    [Window setReleasedWhenClosed:NO];

    [Window makeKeyAndOrderFront:nil];

    Emu.Window->Handle = (__bridge void*)Window;

    ImGui_ImplOSX_Init();
    Emu.CurrentRender().Attach();
    NSEvent* Event = nil;

    for(;;)
    {
        Event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                    untilDate:nil
                                    inMode:NSDefaultRunLoopMode
                                    dequeue:YES];
        if(Event)
        {
            [NSApp sendEvent:Event];
            ImGui_ImplOSX_HandleEvent(Event, [Window contentView]);
        }

        auto& Draw = Emu.CurrentRender();

        Draw.BeginRender();
        ImGui_ImplOSX_NewFrame([Window contentView]);

        ImGui::NewFrame();

        Emu.GUI();

        ImGui::Render();

        Draw.PresentRender();
    }

    Emu.CurrentRender().Detach();
}

#if 0
// basically main on mac
- (void)applicationDidFinishLaunching:(NSNotification*)notify
{
    //Frame::Singleton->PreInit();
    // step 1: create window
    NSWindow* Window = [
        [NSWindow alloc]
        initWithContentRect:NSMakeRect(0, 0, 720, 480)
            styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
            backing:NSBackingStoreBuffered
            defer:NO
    ];

    auto& Emu = Emulator::Get();

    // set basic display
    [Window setTitle:[NSString stringWithUTF8String:Emulator::Get().Window.Title]];
    [Window setAcceptsMouseMovedEvents:YES];
    [Window center];
    [Window setRestorable:YES];

    // this fixes ARC trying to cleanup the window memory after its already been cleaned up
    [Window setReleasedWhenClosed:NO];

    // step 2: show window
    [Window makeKeyAndOrderFront:nil];

    // step 3: attach renderer
    Emu.Window.Handle = (__bridge void*)Window;
    //Frame::Singleton->PostInit();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    puts("here");
    ImGui_ImplOSX_Init();
    puts("and here");
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

        auto& CurrentRender = Emu.CurrentRender();

        // begin render
        CurrentRender.BeginRender();
        puts("before newframe");
        ImGui_ImplOSX_NewFrame([Window contentView]);
        puts("after newframe");

        // do imgui
        ImGui::NewFrame();
        puts("after imgui newframe");
        
        ImGui::Begin("AAA");
        ImGui::End();

        puts("here");
        ImGui::Render();
        puts("past render");
        // present render
        CurrentRender.PresentRender();
    }

    // detach
    Emu.CurrentRender().Detach();
}
#endif

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
        [VApp sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp setDelegate:[VAppDelegate new]];
        [NSApp activateIgnoringOtherApps:YES];
        [NSApp run];
    }
}
