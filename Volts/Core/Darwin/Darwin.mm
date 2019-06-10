
#include "Darwin.h"

#import <Foundation/Foundation.h>

char* Volts::Darwin::OSName()
{
    return strdup([[[NSProcessInfo processInfo] operatingSystemVersionString] UTF8String]);
}