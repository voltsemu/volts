#pragma once

#include "Meta/Macros.h"

namespace Volts::Render
{
    struct Device
    {
        virtual ~Device() {}
        virtual const char* Name() const = 0;
    };

    struct Render
    {
        virtual ~Render() {}
        virtual const char* Name() const = 0;

        virtual const Device* Devices(U32* Count) const = 0;
    };
}