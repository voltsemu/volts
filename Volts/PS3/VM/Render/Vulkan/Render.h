#if WITH_VULKAN

#pragma once

#include <Meta/Macros.h>
#include "PS3/VM/Render/Render.h"
#include "PS3/VM/Render/Frame.h"

#if OS_WINDOWS
#   include <libloaderapi.h>
#endif

namespace Volts::PS3::RSX
{
#if OS_WINDOWS
    using DLL = HMODULE;
#elif OS_LINUX
    using DLL = void*;
#endif

    struct Vulkan : Render
    {
        virtual ~Vulkan() override {}
        virtual void Init() override;
        void Test();

        virtual bool Supported() const override;

        virtual const char* Name() const override { return "Vulkan"; }
        virtual const char* Detail() const override { return "Vulkan is a low level rendering API from the Khronos Group"; }
    private:
        DLL VulkanDLL;
    };
}

#endif