#pragma once

namespace Volts::PS3::RSX
{
    enum class GPUVendor
    {
        AMD, ///< AMD/ATI cards
        NVidia, ///< NVidia cards
        Intel, ///< Intel iGPUs
    };

    struct RenderDevice
    {
        // The name of the device. ie GTX 960, Radeon VII, Intel HD 6000
        char* Name;
    };

    struct Render
    {
        virtual ~Render() {}
        virtual void Init() = 0;
        // is this backend supported on the current platform?
        // IE. does this current platforms GPU support directx 12?
        virtual bool Supported() const = 0;

        virtual const char* Name() const = 0;
        virtual const char* Detail() const = 0;
    };
}