#pragma once

#include "PS3/VM/Render/Render.h"

#import <MetalKit/MetalKit.h>

namespace Volts::PS3::RSX
{
    struct MetalDevice : RenderDevice
    {
        MetalDevice() {}
        MetalDevice(id<MTLDevice> Dev);
        virtual const char* Name() const override;
    private:
        id<MTLDevice> Device;
    };

    struct Metal : Render
    {
        Metal();
        virtual ~Metal() override {}
        virtual InitError Init() override;
        virtual void DeInit() override;
        void Test();

        virtual bool Supported() const override;
        virtual RenderDevice* Devices(unsigned& Count) const override;

        virtual const char* Name() const override { return "Metal"; }
        virtual const char* Detail() const override { return "Metal is a low level rendering API only supported on Apple devices"; }

        virtual bool RequiresDevice() const override { return true; }
    private:
        MetalDevice* AllDevices;
        unsigned DeviceCount;
    };
}
