#pragma once

// this header also works because its all defines
#include "Core/Macros.h"

// we cant use anything except integral types in here because objectiveC++ includes this file
// and objcpp really does not like C++17 code
namespace Volts::PS3::RSX
{
    enum class InitError
    {
        // everything worked
        Ok,
        // driver not found
        NoDriver,
        // driver is unsupported or is missing features
        Unsupported,
        // something else went wrong
        Other,
    };

    struct RenderDevice
    {
        virtual ~RenderDevice() {}
        // The name of the device. ie GTX 960, Radeon VII, Intel HD 6000
        virtual const char* Name() const = 0;
        bool operator==(const RenderDevice& Other) const;
    };

    // call init on these before calling any other function
    struct Render
    {
        virtual ~Render() {}
        // init some basic stuff, dont actually create a window or anything
        virtual InitError Init() = 0;
        virtual void DeInit() = 0;
        // is this backend supported on the current platform?
        // IE. does this current platforms GPU support directx 12?
        virtual bool Supported() const = 0;

        // return an array of RenderDevice and put the length in Count
        virtual RenderDevice* Devices(unsigned& Count) const = 0;

        virtual const char* Name() const = 0;
        virtual const char* Detail() const = 0;

        static void Register(Render* Data);
    };

#define REGISTER_RENDER(Type) auto CAT(__, __LINE__) = (Volts::PS3::RSX::Render::Register(new Type()), 0);
}