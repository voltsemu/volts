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
        Vulkan();

        virtual ~Vulkan() override;
        virtual InitError Init(RenderDevice* Device) override;
        virtual void DeInit() override;

        void Test();

        virtual RenderDevice* Devices(unsigned& Count) const override;
        virtual bool Supported() const override;

        virtual const char* Name() const override { return "Vulkan"; }
        virtual const char* Detail() const override { return "Vulkan is a low level rendering API from the Khronos Group"; }

        virtual bool RequiresDevice() const override { return true; }

    private:

        VkInstance RenderInstance;
#if VVKDEBUG
        VkDebugUtilsMessengerEXT Debugger;
#endif
        Cthulhu::Array<VulkanDevice>* DeviceArray;
    };
}
