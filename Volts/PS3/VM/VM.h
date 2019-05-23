#pragma once

#include "Meta/Aliases.h"
#include "Meta/Macros.h"

#if OS_WINDOWS
#   include <intrin.h>
#else
#   include <xmmintrin.h>
#endif

namespace Volts::PS3
{
    // alias this until i write a concrete class for it
    using Vec128 = __m128;

    // the current state of a thread
    enum class ThreadState : Cthulhu::U8
    {
        None = (1 << 1), ///< the thread hasnt started yet or has shut down cleanly
        Paused = (1 << 2), ///< the thread has been paused by the emulator for an indefinete amount of time
        Frozen = (1 << 3), ///< the thread has been frozen for a short period so save states or other data transfers can occur
        Running = (1 << 4), ///< the thread is running normally
        Dead = (1 << 5), ///< the thread has crashed or has been irreversably broken in some way
    };
}