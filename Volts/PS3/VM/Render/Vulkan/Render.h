#pragma once

#include <Meta/Macros.h>
#include "PS3/VM/Render/Render.h"

#if OS_WINDOWS
#   include <libloaderapi.h>
#endif

namespace Volts::PS3::RSX
{
    struct Vulkan : Render
    {
        void Test();

        virtual void Init() override;

        virtual bool Supported() const override { return VulkanDLL != nullptr; }

        virtual const char* Name() const override { return "Vulkan"; }
        virtual const char* Detail() const override { return "Vulkan is a low level rendering API from the Khronos Group"; }

    private:
        using DLL =
#if OS_WINDOWS
        HMODULE;
#else
        void*;
#endif
        DLL VulkanDLL = nullptr;
    };
}