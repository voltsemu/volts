#pragma once

#include <vector>

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

        virtual bool RequiresDevice() const = 0;
        virtual std::vector<Device> Devices() const = 0;
    };
}