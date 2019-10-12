#pragma once

namespace Volts::Audio
{
    struct Audio
    {
        virtual ~Audio() {}
        virtual const char* Name() const = 0;
    };
}