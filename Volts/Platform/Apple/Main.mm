#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#include "../Interface/EmulatorWindow.h"
#include <Volts/Core/Volts.h>

struct MEmuWindow : Volts::EmulatorWindow
{
    NSWindow* Handle;

    MEmuWindow()
    {
        [NSApplication sharedApplication];

        Handle = [
            [NSWindow alloc]
            initWithContentRect:NSMakeRect(200, 200, 200, 200)
                styleMask:NSWindowStyleMaskResizable | NSWindowStyleMaskClosable | NSWindowStyleMaskTitled
                backing:NSBackingStoreBuffered
                defer:NO
        ];

        [Handle setTitle:@"Volts"];

        auto* View = [NSView new];
        [Handle setContentView:View];
        [Handle setAcceptsMouseMovedEvents:YES];

        [Handle orderFrontRegardless];
        [Handle setRestorable:NO];

        [NSApp run];
    }

    virtual ~MEmuWindow() override {}
    
    virtual void WriteLog(const char* Channel, Volts::Level Severity, const char* Message) override
    {
        NSLog(@"%d %s: %s", Severity, Channel, Message);
    }
};

int main(int argc, const char** argv)
{
    MEmuWindow* Win = new MEmuWindow();
    return VoltsMain(Win);
}
