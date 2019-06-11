#pragma once

#include "PS3/VM/Render/Render.h"

#if OS_APPLE

namespace Volts::PS3::RSX
{
    struct Metal : Render
    {
        virtual ~Metal() override {}
        virtual void Init() override;
        void Test();

        virtual bool Supported() const override;
        virtual RenderDevice* Devices(unsigned& Count) const override;

        virtual const char* Name() const override { return "Metal"; }
        virtual const char* Detail() const override { return "Metal is a low level rendering API only supported on Apple devices"; }
    };

    VSCRIPT({
        Render::Register(new Metal());
    });
}

#endif