#pragma once

namespace Volts
{
    struct Emulator
    {
        virtual ~Emulator() {}

        virtual void Run() = 0;
    };
}