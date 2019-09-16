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
        self.Device = view.device;
        self.Queue = [_Device newCommandQueue];

        // TODO: imgui
    }

    return self;
}

- (void)drawInMTKView:(nonnull MTKView*)view
{
    // TODO
}

- (void)mtkView:(MTKView*)view drawableSizeWillChange:(CGSize)size
{

}

@end

@interface VAppDelegate : NSObject<NSApplicationDelegate>
+ (void)focusApp;
@end

@implementation VAppDelegate

+ (void)focusApp
{
    NSRunningApplication* App = [NSRunningApplication runningApplicationsWithBundleIdentifier:@"com.apple.dock"][0];
    [App activateWithOptions:NSApplicationActivateIgnoringOtherApps];
    //force dock to take focus, this is needed otherwise TransformProcessType will fail
        
    ProcessSerialNumber SerialNumber = { 0, kCurrentProcess };
    (void)TransformProcessType(&SerialNumber, kProcessTransformToForegroundApplication);
    //register the current application as a foreground proccess

    ([[NSRunningApplication currentApplication] activateWithOptions:NSApplicationActivateIgnoringOtherApps]);
    //now make the current app take focus
}


- (void)applicationDidFinishLaunching:(NSNotification*)notify
{
    NSString* BundleName = ([[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleName"]);
    //if were not running in a bundle, force the app to focus (BundleName is nil if not bundled)
    if(BundleName == nil)
        [VAppDelegate focusApp];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)app 
{
    return YES;
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
                styleMask:NSWindowStyleMaskResizable | NSWindowStyleMaskClosable | NSWindowStyleMaskTitled
                backing:NSBackingStoreBuffered
                defer:NO
        ];

        [(NSWindow*)Handle setTitle:@"Volts"];

        View = (MTKView*)((NSWindow*)Handle).contentView;
        [(NSWindow*)Handle setAcceptsMouseMovedEvents:YES];

        [(NSWindow*)Handle orderFrontRegardless];
        [(NSWindow*)Handle setRestorable:NO];

        [NSApplication sharedApplication];
        [NSApp setDelegate:[[VAppDelegate alloc] init]];
        [NSApp run];
#elif OS_LINUX

#endif
    }
}