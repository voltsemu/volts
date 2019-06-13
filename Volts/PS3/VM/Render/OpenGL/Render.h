#pragma once

#include <stdio.h>
#include "PS3/VM/Render/Render.h"

namespace Volts::PS3::RSX
{
    struct OpenGLDevice : RenderDevice
    {

    };

    struct OpenGL : Render
    {
        OpenGL();
        virtual ~OpenGL() override {}

        virtual InitError Init() override;
        virtual void DeInit() override;

        virtual bool Supported() const override { return true; /* i have yet to see a device that doesnt support OpenGL */ }
        virtual RenderDevice* Devices(unsigned& Count) const override { Count = 0; return nullptr; }

        virtual const char* Name() const override { return "OpenGL"; }
        virtual const char* Detail() const override { return "OpenGL is a high level rendering API with great compatibility on windows and linux"; }

        virtual bool RequiresDevice() const override { return false; }
    };
}
