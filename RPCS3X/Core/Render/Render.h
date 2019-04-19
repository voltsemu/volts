#pragma once

#include <Meta/Aliases.h>
#include "Types.h"

namespace RPCS3X::RSX
{
    struct RenderSettings
    {
        // what percentage to upscale by
        U32 Upscale;

        // how strong should the anisotropic filtering be
        U8 Anisotropic;

        // how much should be anitalias
        U8 AntiAlias;

        // what should the frame limit be
        U16 FrameLimit;

        // should we enable vsync
        bool VSync;

        // which device should we use
        // this maps to the id of the device
        U8 Device;
    };

    struct Render
    {
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


        virtual void DoMethod(Object ID, U32 Arg) = 0;

        // called once per frame to tell the renderer to present its image to the window
        virtual void Present() = 0;

        // called whenever the user updates settings
        virtual void ApplySettings(RenderSetttings& Settings) = 0;
    };
}