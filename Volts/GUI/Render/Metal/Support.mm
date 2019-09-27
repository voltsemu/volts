#include "Support.h"

#import <Metal/Metal.h>

#include <locale>
#include <codecvt>

namespace Volts::MetalSupport
{
    std::wstring MetalDevice::Name() const
    {
        return [(__bridge id<MTLDevice>)Handle name].UTF8String;
    }
}