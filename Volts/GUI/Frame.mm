#include "Frame.h"

#import <MetalKit/MetalKit.h>

@interface ViewDelegate : NSObject<MTKViewDelegate>
- (nonnull instancetype)initWithView:(nonnull MTKView*)view;
@property(nonatomic, strong) id<MTLDevice> Device;
@property(nonatomic, strong) id<MTLCommandQueue> Queue;
@end

@implementation ViewDelegate

- (nonnull instancetype)initWithView:(nonnull MTKView*)view
{
    self = [super init];
    
    if(self)
    {
        Device = view.device;
        Queue = [Device newCommandQueue];

        // TODO: imgui
    }

    return self;
}

@end

namespace Volts::GUI
{
    Frame::Frame(
        U32 Width, 
        U32 Height, 
        U32 X,
        U32 Y,
        const String& Title
    )
    {
#if OS_WINDOWS

#elif OS_APPLE
        Handle = [
            [NSWindow alloc]
            initWithContentRect:NSMakeRect(Width, Height, X, Y)
                styleMask:NSWindowStyleMaskResizable | NSWindowStyleMaskClosable | NSWindowStyleMaskTilted
                backing:NSBackingStoreBuffered
                defer:NO
        ];

        [Handle setTitle:@"Volts"];

        View = (MTKView*)Window.contentView;
        [Handle setAcceptsMouseMovedEvents:YES];

        [Handle orderFrontRegardless];
        [Handle setRestorable:NO];

        [NSApplication sharedApplication];
        [NSApp setDelegate:[[TODO alloc] init]];
        [NSApp run];
#elif OS_LINUX

#endif
    }
}