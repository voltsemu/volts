#pragma once

// we have to use basic types in here for various microsoft related issues

namespace Volts
{
    struct EmulatorWindow
    {
        virtual void WriteLog(const char* Channel, char Severity, const char* Message) = 0;
    };
}