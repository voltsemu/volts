#pragma once

#include <Core/Collections/CthulhuString.h>
#include <Core/Collections/Array.h>
#include <Core/Types/Lambda.h>

#include "imgui/imgui.h"

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
        static Frame* Singleton;
        static Frame* GetSingleton();

        Frame();
        void Run();

        Frame* SetTitle(const char* T);
        const char* GetTitle() const;
    private:
        const char* Title = nullptr;
    public:

        ImGuiTextBuffer LogBuffer;
        void Log(std::string Msg);

        void GUILoop();

#if OS_WINDOWS
        using FrameHandle = HWND;

        void SetChild(HWND C);
#elif OS_LINUX
        using FrameHandle = void*; // ???
#elif OS_APPLE
        using FrameHandle = void*; // NSView*
#endif

        void FinalizeRenders();
        Array<RSX::Render*> Renders = {};
        const char** RenderNames = nullptr;
        U32 RenderCount = 0;


        void FinalizeDevices();
        const char** DeviceNames = nullptr;
        U32 DeviceCount = 0;

        RSX::Render* CurrentRender = nullptr;

        Size GetSize() const;
        AspectRatio Aspect = { 16, 9 };
        FrameHandle Handle;
    };
}