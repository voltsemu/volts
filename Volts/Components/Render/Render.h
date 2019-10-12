#pragma once

namespace Volts::Render
{
    struct Render
    {
        virtual ~Render() {}
        virtual const char* Name() const = 0;
    };
}