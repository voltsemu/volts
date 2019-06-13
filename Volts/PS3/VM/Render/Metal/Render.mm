
#include "Render.h"

namespace Volts::PS3::RSX
{
    MetalDevice::MetalDevice(id<MTLDevice> Dev)
    {
        Device = Dev;
    }

    Metal::Metal()
    {
        Render::Register(this);
    }

    const char* MetalDevice::Name() const
    {
        return [Device.name UTF8String];
    }

    InitError Metal::Init()
    {
        // make sure we're supported on this system
        if(!Supported())
            return InitError::NoDriver;

        // get all devices
        NSArray<id<MTLDevice>>* Devs = MTLCopyAllDevices();

        DeviceCount = [Devs count];
        AllDevices = new MetalDevice[DeviceCount];

        for(unsigned I = 0; I < DeviceCount; I++)
        {
            AllDevices[I] = MetalDevice(Devs[I]);
        }

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