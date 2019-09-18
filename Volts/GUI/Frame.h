#pragma once

#include <Core/Collections/CthulhuString.h>
#include <Core/Types/Lambda.h>

#include <chrono>

#if OS_WINDOWS
#   include <Windows.h>
#   include <d3d12.h>
#   include <dxgi1_4.h>
#elif OS_LINUX
// TODO: linux includes
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
        void Run();

        static void GUILoop(Frame* F);

#if OS_WINDOWS
        using FrameHandle = HWND;
#elif OS_APPLE
        using FrameHandle = void*; // NSWindow
#elif OS_LINUX
        using FrameHandle = void*; // IDK
#endif

#if !OS_APPLE
        // apple only supports metal so we dont need a pointer to a renderer
        RSX::Render* CurrentRender = nullptr;
#endif

        std::chrono::time_point<std::chrono::high_resolution_clock> LastFrame = std::chrono::high_resolution_clock::now();

        FrameHandle Handle;
        String T;
    };
}