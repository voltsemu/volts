#pragma once

#include <Cthulhu/Meta/Aliases.h>
#include <Cthulhu/Core/Collections/Array.h>
#include <Cthulhu/Core/Collections/CthulhuString.h>
#include "Types.h"

namespace RPCS3X::RSX
{
    using namespace Cthulhu;
    using RenderState = void*;

    using GPUDeviceID = U8;
    using GPUDevices = Cthulhu::Array<GPUDeviceID>;

    using Str = Cthulhu::String;

    struct Render
    {
        // ==================================================
        //                [ Window Managment ]
        // ==================================================

        // called to initialize the render state
        virtual void Initialize() = 0;

        // called to clean up renderer
        virtual void Close() = 0;

        // called when the window is resized
        virtual void Resize(U32 Width, U32 Height) = 0;

        // called when the window is minimized
        virtual void Minimize() = 0;

        // called when the window is maximized
        virtual void Maximize() = 0;

        // ==================================================
        //              [ Emulator Interface ]
        // ==================================================

        // tell the renderer to pause rendering and wait
        virtual void Pause() = 0;

        // tell the renderer to start where it left off and keep rendering
        virtual void Resume() = 0;


        // save render state to be loaded from later
        virtual RenderState SaveState() const = 0;

        // load render state from previous save state
        virtual void LoadFromState(RenderState State) = 0;

        // get pretty name of backend
        virtual Str Name() const = 0;

        // get pretty description of backend
        virtual Str Description() const = 0;

        // ==================================================
        //                   [ Interface ]
        // ==================================================

        // called once per frame to tell the renderer to present its image to the window
        virtual void Present() = 0;


        // ==================================================
        //                    [ Settings ]
        // ==================================================

        // apply a new upscaling setting
        virtual void ApplyUpscale(U32 Scale) = 0;

        // get the current upscaling factor
        virtual U32 ResolutionScale() const = 0;


        // set new anisotropic filtering multiplyer
        virtual void ApplyAnisotropic(U8 Level) = 0;

        // get the current anisotropic filtering factor
        virtual U8 AnisotropicStrength() const = 0;


        // set new strength for anti aliasing
        virtual void ApplyAntiAlias(U8 Level) = 0;

        // get current strength of antialiasing
        virtual U8 AliasStength() const = 0;


        // apply a new fps limit
        virtual void ApplyFramelimit(U16 Limit) = 0;

        // get the current frame limit
        virtual U16 FrameLimit() const = 0;


        // apply a new vsync setting
        virtual void ApplyVSync(bool Sync) = 0;

        // check if vsync is currently enabled
        virtual bool UsingVSync() const = 0;


        // switch to a different gpu device
        virtual void SwitchDevice(GPUDeviceID ID) = 0;

        // get an array of all compatible devices
        virtual GPUDevices CompatibleDevices() const = 0;

        virtual void FreeDevices(GPUDevices Devices) const = 0;

        // get the pretty name of a device
        virtual Str DeviceName(const GPUDeviceID& Device) const = 0;
    };

    using RenderInfo = Render*;

    using RenderBackends = Cthulhu::Array<RenderInfo>;

    static RenderBackends Backends;
    Empty AddBackend(const RenderInfo Info);

#   define TOKEN_PASTE(X, Y) X##Y
#   define CAT(X, Y) TOKEN_PASTE(X, Y)
#   define REGISTER_RENDER(Type) namespace { static auto CAT(RR, __LINE__) = AddBackend(new Type()); }
}