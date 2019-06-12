#if WITH_VULKAN

#pragma once

#include <Meta/Macros.h>
#include "VulkanSupport.h"

#if OS_WINDOWS
#   include <libloaderapi.h>
#endif

namespace Volts::PS3::RSX
{
    struct Vulkan : Render
    {
        virtual ~Vulkan() override {}
        virtual InitError Init() override;
        virtual void DeInit() override;

        void Test();

        virtual RenderDevice* Devices(unsigned& Count) const override;
        virtual bool Supported() const override;

        virtual const char* Name() const override { return "Vulkan"; }
        virtual const char* Detail() const override { return "Vulkan is a low level rendering API from the Khronos Group"; }

    private:
        VkInstance RenderInstance;
        Cthulhu::Array<VulkanDevice> DeviceArray;
    };

    VSCRIPT({
        Render::Register(new Vulkan());
    });
}

#endif