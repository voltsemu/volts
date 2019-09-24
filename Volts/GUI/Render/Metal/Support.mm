#include "Support.h"

#import <Metal/Metal.h>

#include <locale>
#include <codecvt>

namespace Volts::MetalSupport
{
    std::wstring MetalDevice::Name() const
    {
        return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>()
            .from_bytes([(__bridge id<MTLDevice>)Handle name].UTF8String);
    }
}