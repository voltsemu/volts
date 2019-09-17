#include "Frame.h"

#import <Cococa/Cocoa.h>
#import <MetalKit/MetalKit.h>

@interface VApp : NSApplication
@end
@implementation VApp
- (void)sendEvent:(NSEvent*)event
{
    if([event type] == NSEventTypeKeyUp && ([event modifierFlags] & NSEventModiferFlagCommand))
        [[self keyWindow] sendEvent:event];
    else
        [super selfEvent:event];
}
@end

@interface VWindowDelegate : NSObject<NSWindowDelegate>
@end
@implementation VWindowDelegate
@end

@interface VViewDelegate : NSObject<NSViewDelegate>
@end
@implementation VViewDelegate
@end

@interface VMTKView : MTKView
@end
@implementation VMTKView
@end

@interface VAppDelegate : NSObject<NSApplicationDelegate>
@end
@implementation VAppDelegate
- (void)applicationDidFinishLaunching:(NSNotification*)notify
{

}
@end

namespace Volts::GUI
{
    Frame::Frame()
    {
        Handle = [
            [NSWindow alloc]
        ]
    }

    Frame& Frame::Title(const String& T) { this->T = T; return *this; }

    void Frame::Run(Lambda<void()> Generator)
    {
        [VApp sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp setDelegate:[[VAppDelegate alloc] init]];
        [NSApp activateIgnoringOtherApps:YES];
        [NSApp run];
    }
}