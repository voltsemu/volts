#pragma once

#include <stdint.h>

#ifdef _WIN32
#   define OS_WINDOWS 1
#elif defined(__APPLE__)
#   define OS_APPLE 1
#elif defined(__linux__)
#   define OS_LINUX 1
#endif

#ifndef OS_WINDOWS
#   define OS_WINDOWS 0
#endif

#ifndef OS_APPLE
#   define OS_APPLE 0
#endif

#ifndef OS_LINUX
#   define OS_LINUX 0
#endif

namespace Volts
{
    using U8 = uint8_t;
    using U16 = uint16_t;
    using U32 = uint32_t;
    using U64 = uint64_t;

    using I8 = int8_t;
    using I16 = int16_t;
    using I32 = int32_t;
    using I64 = int64_t;

    using F32 = float;
    using F64 = double;
}