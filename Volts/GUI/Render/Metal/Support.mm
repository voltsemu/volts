#include "Support.h"

#import <Metal/Metal.h>

#include <locale>
#include <codecvt>

namespace Volts::MetalSupport
{
    const char* MetalDevice::Name() const
    {
        return [(id<MTLDevice>)Handle name].UTF8String;
    }
}