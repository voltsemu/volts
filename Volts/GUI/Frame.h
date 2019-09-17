#pragma once

#include <Core/Collections/CthulhuString.h>
#include <Core/Types/Lambda.h>

#if OS_WINDOWS
#   include <Windows.h>
#   include <d3d12.h>
#   include <dxgi1_4.h>
#elif OS_APPLE
// TODO: what includes
#elif OS_LINUX
// TODO: window includes
#endif

namespace Volts::RSX
{
    struct Render;
}

namespace Volts::GUI
{
    using namespace Cthulhu;

#if OS_WINDOWS
    extern HINSTANCE Instance;
#endif

    struct Frame
    {
        Frame();
        Frame& Title(const String& T);
        void Run(Lambda<void()> Generator);
#if OS_WINDOWS
        using FrameHandle = HWND;
#elif OS_APPLE
        using FrameHandle = void*; // NSWindow
#elif OS_LINUX
        using FrameHandle = void*; // IDK
#endif
        FrameHandle Handle;
        RSX::Render* CurrentRender = nullptr;
        String T;
    };
}