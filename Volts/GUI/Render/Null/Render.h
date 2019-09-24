#pragma once

#include "Render/Render.h"

namespace Volts::RSX
{
    struct Null : Render
    {
        Null() {}
        virtual ~Null() {}
        virtual void Attach(GUI::Frame* Frame) override {}
        virtual void Detach() override {}

        virtual const String Name() const override { return "Null"; }
        virtual const String Description() const override { return "Null"; }
        virtual Device* Devices(U32* Count) override { return nullptr; }

        virtual void Windowed() override {}
        virtual void Fullscreen() override {}
        virtual void Borderless() override {}

        virtual void Resize(GUI::Size NewSize) override {}

        virtual void BeginRender() override {}
        virtual void PresentRender() override {}
    };
}