#pragma once

#include <stddef.h>

// we have to use basic types in here for various microsoft related issues

namespace Volts
{
    enum class Level : short
    {
        Trace,
        Info,
        Debug,
        Warning,
        Error,
        Fatal
    };

    // used for calling the frontend to do stuff
    struct Native
    {
        virtual ~Native() {}

        // write to log
        virtual void WriteLog(const char* Channel, Level Severity, const char* Message) = 0;
    };
}

