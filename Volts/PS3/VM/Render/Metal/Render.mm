
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

    bool Metal::Supported() const
    {
        return !!MTLCreateSystemDefaultDevice();
    }
}