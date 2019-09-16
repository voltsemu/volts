#include "Frame.h"

#include "imgui/imgui.h"

#if OS_WINDOWS
#   include "imgui/examples/imgui_impl_dx12.h"
#   include "imgui/examples/imgui_impl_win32.h"
#else
#   include <imgui/examples/imgui_impl_vulkan.h>
#endif

namespace Volts::GUI
{
#if OS_WINDOWS
    HINSTANCE Instance = {};

    Frame::Frame(
        U32 W,
        U32 H,
        U32 X,
        U32 Y,
        const String& Title
    )
    {
        WNDCLASSEX WC = {};
        WC.cbSize = sizeof(WNDCLASSEX);
        WC.style = CS_HREDRAW | CS_VREDRAW;
        WC.lpfnWndProc = DefWindowProc;
        WC.hInstance = Instance;
        WC.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
        WC.hCursor = LoadCursor(nullptr, IDC_ARROW);
        WC.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        WC.lpszClassName = L"Volts";
        WC.hIconSm = LoadIcon(nullptr, IDI_WINLOGO);

        RegisterClassEx(&WC);

        I32 Width = GetSystemMetrics(SM_CXSCREEN);
        I32 Height = GetSystemMetrics(SM_CYSCREEN);

        // TODO: everything
    }
#else

#endif
}