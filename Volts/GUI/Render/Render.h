#pragma once

#include <Core/Collections/CthulhuString.h>

#include "Frame.h"

namespace Volts::GUI
{
    struct Frame;
}

#if OS_APPLE
namespace Volts::RSX
{
    using ViewHandle = void*; // metal requires a view to be passed around when rendering
}

#   define VIEW_ARG(Name) Volts::RSX::ViewHandle Name
#else
#   define VIEW_ARG(_)
#endif

namespace Volts::RSX
{
    using namespace Cthulhu;

    struct Device
    {
        virtual ~Device() {}
        virtual const char* Name() const = 0;
    };

    // each renderer can be fed a window that it will render to
    struct Render
    {
        virtual ~Render() {}
        // this starts the renderer and gives it the window handle it needs
        virtual void Attach(GUI::Frame* Handle) = 0;
        // this detaches the renderer from the window and shuts down the backend
        virtual void Detach() = 0;

        virtual void UpdateVSync(bool Enabled) = 0;

        virtual const char* Name() const = 0;
        virtual const char* Description() const = 0;
        virtual bool Supported() const = 0;

        virtual Device* Devices(U32* Count) = 0;

        virtual void Windowed() = 0;
        virtual void Fullscreen() = 0;
        virtual void Borderless() = 0;

        virtual void Resize(GUI::Size NewSize) = 0;

        virtual void BeginRender(VIEW_ARG(_)) = 0;
        virtual void PresentRender() = 0;
    };
}