#pragma once

namespace Volts::Input
{
    struct Controller
    {
        virtual ~Controller() {}
        virtual const char* Name() const = 0;
    };
}