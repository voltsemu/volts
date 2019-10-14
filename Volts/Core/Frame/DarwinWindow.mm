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

    void Window::Close()
    {
        
    }

    void Window::Open()
    {
        [VApp sharedApplication];
        [NSApp run];
    }
}