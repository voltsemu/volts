#if WITH_VULKAN

#include <Core/Collections/CthulhuString.h>
#include "Render.h"

namespace Volts::PS3::RSX
{
    using namespace Cthulhu;

    InitError Vulkan::Init()
    {
        auto Instance = VulkanSupport::MakeInstance();

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
        auto Inst = VulkanSupport::SimpleInstance();
        auto Devs = VulkanSupport::Devices(Inst);
    }

    RenderDevice* Vulkan::Devices(unsigned& Count) const
    {
        return nullptr;
    }

    bool Vulkan::Supported() const
    {
        return true;
    }
}

#endif // WITH_VULKAN