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
        // this starts the renderer and gives it the window handle it needs
        virtual void Start(GUI::Frame* Handle) = 0;
        // this detaches the renderer from the window and shuts down the backend
        virtual void Detach() = 0;

        virtual const String& Name() const = 0;
        virtual const String& Description() const = 0;

        virtual void InitGUI() const = 0;
        virtual void NewGUIFrame() const = 0;
        virtual void ShutdownGUI() const = 0;
        virtual void RenderGUI() const = 0;
    };
}