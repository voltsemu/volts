#pragma once

namespace Volts::PS3::RSX
{
    struct Render
    {
        virtual void Init();
        // is this backend supported on the current platform?
        // IE. does this current platforms GPU support directx 12?
        virtual bool Supported() const = 0;

        virtual const char* Name() const = 0;
        virtual const char* Detail() const = 0;
    };
}