
#include "Render.h"

#import <simd/simd.h>

@implementation Copper
{
    id<MTLDevice> Device;
    id<MTLRenderPipelineState> State;
    id<MTLCommandQueue> Queue;
    vector_uint2 Size;
}

- (void)mtkView:(nonnull MTKView*)view drawableSizeWillChange:(CGSize)size
{

}

- (void)drawInMTKView:(nonnull MTKView*)view
{

}

- (instancetype)initWithView:(MTKView*)view
{
    self = [super init];

    if(self)
    {
        NSError* Error = nullptr;

        //Device = view.device;

        id<MTLLibrary> Lib = [Device newDefaultLibrary];

        MTLRenderPipelineDescriptor* Desc = [MTLRenderPipelineDescriptor new];
        Desc.label = @"Volts";

        State = [Device newRenderPipelineStateWithDescriptor:Desc error:&Error];

        Queue = [Device newCommandQueue];
    }

    return self;
}

- (void)MTKView:(nonnull MTKView*)view drawableSizeWillChange:(CGSize)size
{
    Size.x = size.width;
    Size.y = size.height;
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

@end

namespace Volts::PS3::RSX
{
    MetalDevice::MetalDevice(id<MTLDevice> Dev)
    {
        Device = Dev;
    }

    Metal::Metal()
    {
        Render::Register(this);

        // make sure we're supported on this system
        if(!Supported())
            return;

        // get all devices
        NSArray<id<MTLDevice>>* Devs = MTLCopyAllDevices();

        DeviceCount = [Devs count];
        AllDevices = new MetalDevice[DeviceCount];

        for(unsigned I = 0; I < DeviceCount; I++)
            AllDevices[I] = MetalDevice(Devs[I]);
    }

    std::wstring MetalDevice::Name() const
    {
        auto Str = [Device.name UTF8String];
        return std::wstring(&Str[0], &Str[[Device.name length]]);
    }

    InitError Metal::Init(RenderDevice* Device)
    {
        if(!Supported())
            return InitError::NoDriver;

        Window = [
            [NSWindow alloc]
            initWithContentRect:NSMakeRect(500, 500, 500, 500)
                styleMask:NSWindowStyleMaskResizable | NSWindowStyleMaskClosable | NSWindowStyleMaskTitled
                backing:NSBackingStoreBuffered
                defer:NO
        ];

        [Window setTitle:@"Metal Volts"];

        View = (MTKView*)Window.contentView;
        [Window setAcceptsMouseMovedEvents:YES];

        //[Window orderFrontRegardless];
        //[Window setRestorable:NO];

        [NSApplication sharedApplication];
        [NSApp setDelegate:[[VAppDelegate alloc] init]];
        [NSApp run];

        return InitError::Ok;
    }

    void Metal::DeInit()
    {

    }

    void Metal::Test()
    {

    }

    RenderDevice* Metal::Devices(unsigned& Count) const
    {
        Count = DeviceCount;
        return AllDevices;
    }

    bool Metal::Supported() const
    {
        // only returns a valid device if metal is supported
        // otherwise it returns null, which is also false
        return !!MTLCreateSystemDefaultDevice();
    }

    static Metal* MTLSingleton = new Metal();
}