#pragma once

#include "Meta/Macros.h"

namespace Volts::Render
{
    using namespace Cthulhu;
    
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
        virtual void SetDevice(U32 Index) = 0;

        virtual void Attach() = 0;
        virtual void Detach() = 0;

        virtual void Begin() = 0;
        virtual void End() = 0;
    };
}