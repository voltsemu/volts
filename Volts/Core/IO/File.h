#pragma once

#include <Meta/Macros.h>

#if OS_WINDOWS
#   include <fileapi.h>
#else
#   include <stdio.h>
#endif

namespace Volts
{
    struct File
    {
#if OS_WINDOWS
        HANDLE Handle;
#else
        FILE* Handle;
#endif
    };

    File Open();
}