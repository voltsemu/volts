#pragma once

// we have to use basic types in here for various microsoft related issues

namespace Volts
{
#if defined(_WIN32) || defined(_WIN64)
    public ref class EmulatorWindow
#else
    class EmulatorWindow
#endif
    {
    public:
        virtual void WriteLog(const char* Channel, char Severity, const char* Message) = 0;
    };
}