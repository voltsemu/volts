
#include "Render.h"

#import <simd/simd.h>

@implementation Copper
{
    id<MTLDevice> Device;
    id<MTLRenderPipelineState> State;
    id<MTLCommandQueue> Queue;
    vector_uint2 Size;
}

- (nonnull instancetype)initWithView:(nonnull MTLView*)view
{
    self = [super init];

    if(self)
    {
        NSError* Error = nullptr;

        Device = view.device;

        id<MTLLibrary> Lib = [Device newDefaultLibrary];

        MTLRenderPipelineDescriptor* Desc = [MTLRenderPipelineDescriptor new];
        Desc.label = @"Volts";
        Desc.colorAttatchments[0].pixelFormat = view.colorPixelFormat;

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