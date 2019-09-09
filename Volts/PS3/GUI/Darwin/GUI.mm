#include "GUI.h"

#import <Cocoa/Cocoa.h>

namespace Volts::PS3
{
    GUI::GUI()
    {   
        NSWindow* Window = [
            [NSWindow alloc]
            initWithContentRect:NSMakeRect(500, 500, 500, 500)
                styleMask:NSWindowStyleMaskResizable | NSWindowStyleMaskClosable | NSWindowStyleMaskTitled
                backing:NSBackingStoreBuffered
                defer:NO
        ];

        [Window setTitle:@"Volts"];
        [Window setAcceptsMouseMovedEvents:YES];

        [Window orderFrontRegardless];
        [Window setRestorable:NO];

        Handle = Window;

        [NSApplication sharedApplication];
        [NSApp run];
    }
}