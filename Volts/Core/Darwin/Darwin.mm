#include "Darwin.h"

Cthulhu::String Volts::Darwin::OSName()
{
    return [[[NSProcessInfo processInfo] operatingSystemVersionString] UTF8String];
}