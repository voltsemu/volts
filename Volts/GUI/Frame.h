#pragma once

#include <Core/Collections/CthulhuString.h>
#include <Core/Types/Lambda.h>

#include "imgui/imgui.h"

#include <chrono>
#include <deque>
#include <string>

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

    struct Size
    {
        U32 Width, Height;
    };

    using AspectRatio = Size;

    enum WindowState
    {
        Windowed,
        Borderless,
        Fullscreen
    };

    struct Frame
    {
        Frame();
        Frame& Title(const String& T);
        void Run();

        void SetBackground(const String& Path);

        void Fullscreen();
        void Borderless();
        void Windowed();

        static ImGuiTextBuffer Logs;
        static void GUILoop(Frame* F);
        static void Log(std::string Msg);

#if OS_WINDOWS
        using FrameHandle = HWND;
#elif OS_APPLE
        using FrameHandle = void*; // NSWindow
#elif OS_LINUX
        using FrameHandle = void*; // IDK
#endif

#if !OS_APPLE
        // apple only supports metal so we dont need a pointer to a renderer
        static RSX::Render* CurrentRender;
#endif

        std::chrono::time_point<std::chrono::high_resolution_clock> LastFrame = std::chrono::high_resolution_clock::now();

        Size GetSize() const;

        AspectRatio Aspect = { 16, 9 };

        U32 VSync = false;
        FrameHandle Handle;
        String T;
    };
}