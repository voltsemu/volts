#pragma once

#include "Support.h"

#include "Frame.h"

#include "imgui/imgui.h"

namespace Volts::RSX
{
    struct Metal : Render
    {
        Metal();
        virtual ~Metal() {}
        virtual void Attach(GUI::Frame* Frame) override;
        virtual void Detach() override;

        virtual const String Name() const override { return "Metal"; }
        virtual const String Description() const override { return "MetalKit"; }
        virtual Device* Devices(U32* Count) override;

        virtual void Windowed() override;
        virtual void Fullscreen() override;
        virtual void Borderless() override;

        virtual void Resize(GUI::Size NewSize) override;

        virtual void BeginRender() override;
        virtual void PresentRender() override;
    private:
        GUI::Frame* Frame;
        ImGuiIO& IO;

        void* Handle;
    };
}