#pragma once

namespace Volts::FileSystem
{
    struct Manager
    {
        virtual ~Manager() {}
        virtual const char* Name() const = 0;
    };
}