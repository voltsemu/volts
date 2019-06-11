
#include "Render.h"

#import <MetalKit/MetalKit.h>

namespace Volts::PS3::RSX
{
    void Metal::Init()
    {

    }

    void Metal::Test()
    {
        NSArray<id<MTLDevice>>* Devices = MTLCopyAllDevices();
        for(id<MTLDevice> Device : Devices)
        {
            NSLog(@"name %@", Device.name);
        }
    }

    RenderDevice* Metal::Devices(unsigned& Count) const
    {
        Count = 0;
        return nullptr;
    }

    bool Metal::Supported() const
    {
        // only returns a valid device if metal is supported
        // otherwise it returns null, which is also false
        return !!MTLCreateSystemDefaultDevice();
    }
}