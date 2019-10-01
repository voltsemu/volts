#pragma once

#include "Render/Render.h"
#include "Frame.h"

namespace Volts::RSX
{
    struct Null : Render
    {
        Null() { GUI::Frame::Renders.Append((void*)this); }
        virtual ~Null() {}
        virtual void Attach(GUI::Frame* Frame) override {}
        virtual void Detach() override {}

        virtual const char* Name() const override { return "Null"; }
        virtual const char* Description() const override { return "Null"; }
        virtual Device* Devices(U32* Count) override { return nullptr; }
        virtual void UpdateVSync(bool Enabled) override {}

        virtual void Windowed() override {}
        virtual void Fullscreen() override {}
        virtual void Borderless() override {}

        virtual void Resize(GUI::Size NewSize) override {}

        virtual void BeginRender() override {}
        virtual void PresentRender() override {}
    };
}