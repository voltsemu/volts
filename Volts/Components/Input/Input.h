#pragma once

namespace Volts::Input
{
    struct Input
    {
        virtual ~Input() {}
        virtual const char* Name() const = 0;
    };
}