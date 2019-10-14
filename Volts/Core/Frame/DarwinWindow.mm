#include "Window.h"

#import <Cocoa/Cocoa.h>

@interface VApp : NSApplication
@end

@implementation VApp
@end

namespace Volts
{
    void Window::CreateFrame()
    {

    }

    void Window::Show()
    {
        [VApp sharedApplication];
        [NSApp run];
    }
}