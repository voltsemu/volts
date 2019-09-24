#include "Support.h"

#import <Metal/Metal.h>

namespace Volts::MetalSupport
{
    std::wstring MetalDevice::Name() const
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>()
            .from_bytes([(MTLDevice*)Handle name].UTF8String);
    }
}