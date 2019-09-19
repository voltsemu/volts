#pragma once

#include <Core/Collections/CthulhuString.h>

namespace Volts::GUI
{
    struct Frame;
}

namespace Volts::RSX
{
    using namespace Cthulhu;

    // each renderer can be fed a window that it will render to
    struct Render
    {
        virtual ~Render() {}
        // this starts the renderer and gives it the window handle it needs
        virtual void Attach(GUI::Frame* Handle) = 0;
        // this detaches the renderer from the window and shuts down the backend
        virtual void Detach() = 0;

        virtual const String Name() const = 0;
        virtual const String Description() const = 0;

        virtual void BeginRender() = 0;
        virtual void PresentRender() = 0;
    };
}