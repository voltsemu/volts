#pragma once

#include "PS3/VM/Render/Render.h"

#import <MetalKit/MetalKit.h>

// make objcpp shut up about sticking _Nonnull and _Nullable in front of every damn pointer
// TODO: once code is stabilized remove this and mark everythings nullability
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"

// objective c classes can only appear in global scope
@interface Copper : NSObject<MTKViewDelegate>

- (nonnull instancetype)initWithView:(nonnull MTKView*)view;

@end

namespace Volts::PS3::RSX
{
    struct MetalDevice : RenderDevice
    {
        MetalDevice() {}
        MetalDevice(id<MTLDevice> Dev);
        virtual std::wstring Name() const override;
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
        MTKView* _Nonnull View;
        unsigned DeviceCount;
    };
}

#pragma clang diagnostic pop