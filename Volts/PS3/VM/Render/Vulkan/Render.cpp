#if WITH_VULKAN

#include <Core/Collections/CthulhuString.h>
#include "Render.h"

namespace Volts::PS3::RSX
{
    using namespace Cthulhu;

    InitError Vulkan::Init()
    {
        // if the library cant be opened, chances are it doesnt exist
        if(!VulkanSupport::LoadDLL())
            return InitError::NoDriver;

        const char* Extensions[] = {
            VK_KHR_SURFACE_EXTENSION_NAME,
#if OS_WINDOWS
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif OS_LINUX
            VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
#if VK_USE_PLATFORM_WAYLAND_KHR
            VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME
#endif // VK_USE_PLATFORM_WAYLAND_KHR
#endif // OS_WINDOWS OS_LINUX
        };

        // everything went well
        return InitError::Ok;
    }

#if OS_WINDOWS
    LRESULT CALLBACK WindowProc(HWND Wnd, UINT Msg, WPARAM W, LPARAM L)
    {
        switch(Msg)
        {
        case WM_KEYDOWN:
            break;
        case WM_KEYUP:
            break;
        default:
            return DefWindowProc(Wnd, Msg, W, L);
        }

        return 0;
    }
#endif

    void Vulkan::Test()
    {
    }

    RenderDevice* Vulkan::Devices(unsigned& Count) const
    {
        return nullptr;
    }

    bool Vulkan::Supported() const
    {
        return VulkanSupport::Found();
    }
}

#endif // WITH_VULKAN