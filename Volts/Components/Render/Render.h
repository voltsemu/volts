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

        // user interaction for devices
        virtual const Device* Devices(U32* Count) const = 0;
        virtual void SetDevice(U32 Index) = 0;

        // attaching and detaching from frame
        virtual void Attach() = 0;
        virtual void Detach() = 0;

        // render loop
        virtual void Begin() = 0;
        virtual void End() = 0;

        // Window interaction
        virtual void Resize(U32 Width, U32 Height) = 0;
    };
}