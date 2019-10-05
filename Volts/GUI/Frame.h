#pragma once

#include <Core/Collections/CthulhuString.h>
#include <Core/Collections/Array.h>
#include <Core/Types/Lambda.h>

#include "Core/Macros.h"

#include "Core/Logger/Logger.h"

#include "GUIExtensions.h"

#include <chrono>

#if OS_WINDOWS
#   include <Windows.h>
#   include <d3d12.h>
#   include <dxgi1_4.h>
#elif OS_LINUX
// TODO: linux includes
#endif

#define OBJC_CLASS(Name, Super) @interface Name : Super @end @implementation Name

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
        ~Frame();
        void Run();

        Frame* SetTitle(const char* T);
        const char* GetTitle() const;
    private:
        const char* Title = nullptr;
    public:

        ImGuiTextBuffer LogBuffer;
        Level CurrentLevel = Level::Info;
        void Log(Level L, const char* Msg);

        void GUILoop();

#if OS_WINDOWS
        using FrameHandle = HWND;

        void SetChild(HWND C);
#elif OS_LINUX
        using FrameHandle = void*; // ???
#elif OS_APPLE
        using FrameHandle = void*; // ???
#endif

        void FinalizeRenders();
        void AddRender(RSX::Render* R);
        Array<RSX::Render*> Renders = {};

        I32 RenderIndex = 0;
        const char** RenderNames = nullptr;
        U32 RenderCount = 0;

        struct
        {
            TimePoint LastFrame;
        } Metrics;

        void UpdateDevices();
        I32 DeviceIndex = 0;
        const char** DeviceNames = nullptr;
        U32 DeviceCount = 0;

        void SetRender(const char* Name);
        RSX::Render* CurrentRender = nullptr;

        Size GetSize() const;
        AspectRatio Aspect = { 16, 9 };
        FrameHandle Handle;
    };
}