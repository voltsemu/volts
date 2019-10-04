#pragma once

#include "Render/Render.h"

#if OS_WINDOWS
#   include <GL/gl.h>
#elif OS_LINUX
#   include <GL/gl.h>
#elif OS_APPLE
#   include <OpenGL/gl.h>
#endif

namespace Volts::RSX
{
    struct OpenGL : Render
    {
        OpenGL();
        virtual ~OpenGL() override;

        virtual void Attach(GUI::Frame* Handle) override;
        virtual void Detach() override;

        virtual void UpdateVSync(bool Enabled) override;

        virtual const char* Name() const override { return "OpenGL"; }
        virtual const char* Description() const override { return "OpenGL"; }

        // TODO: for now opengl isnt working
        virtual bool Supported() const override { return false; }

        virtual Device* Devices(U32* Count) override;

        // opengl doesnt need a device so this can be stubbed
        virtual void SetDevice(RSX::Device* Device) override {}

        virtual void Windowed() override;
        virtual void Fullscreen() override;
        virtual void Borderless() override;

        virtual void Resize(GUI::Size NewSize) override;

        virtual void BeginRender(VIEW_ARG(_)) override;
        virtual void PresentRender() override;
    private:
        GUI::Frame* Frame;
    };
}