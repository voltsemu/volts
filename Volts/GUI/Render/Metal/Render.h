#pragma once

#include "Render/Render.h"

namespace Volts::RSX
{
    struct Metal : Render
    {
        virtual ~Metal() {}
        virtual void Start(GUI::Frame* Handle) override;
        virtual void Detach() override;
        virtual const String Name() const override { return "Metal"; }
        virtual const String Description() const override { return "MetalKit"; }

        virtual void InitGUI() const override
        {
            // TODO
        }

        virtual void NewGUIFrame() const override
        {
            // TODO
        }

        virtual void ShutdownGUI() const override
        {
            // TODO
        }

        virtual void RenderGUI() const override
        {
            // TODO
        }

    private:
        bool Setup();

        bool CreateInstance();

        void* Handle;
    };
}