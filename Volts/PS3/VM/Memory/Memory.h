#pragma once

#include "Meta/Aliases.h"

namespace Volts::PS3
{
    // the ps3 had 512MB of memory which is split into a few distinct sections
    // theres 256 MB of main memory which the cell chip had fast access to
    // theres another 256MB of RSX memory which could be accessed by both
    // the RSX and CELL chips but was faster for the RSX to access
    // this isnt the case in here because of emulation stuff but may
    // be important in some cases

    // the ps3s global memory space
    extern Cthulhu::Byte* GlobalMemory;

    // call at the begining of the ps3 emulation
    void InitGlobalMemory();

    // call at the end of the ps3 emulation to stop memory leaks
    void CleanupGlobalMemory();

    // 512 megabytes
    constexpr Cthulhu::U32 GlobalMemorySize = 512 * 1024 * 1024;
}