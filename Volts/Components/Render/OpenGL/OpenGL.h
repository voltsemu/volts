#pragma once

#include "Core/Emulator.h"

namespace Volts::Render
{
    struct OpenGL : Render
    {
        OpenGL();
        virtual ~OpenGL() override {}

        // opengl doesnt support devices so we can stub these
        virtual const Device* Devices(U32* Count) override
        {
            *Count = 0;
            return nullptr;
        }

        virtual void SetDevice(U32 Index) override {}

        virtual void Attach() override;
        virtual void Detach() override;

        virtual void Begin() override;
        virtual void End() override;
    };
}