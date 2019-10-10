#pragma once

#include <Core/Collections/CthulhuString.h>
#include <Core/Collections/Array.h>
#include <Core/Types/Lambda.h>

#include "Core/Macros.h"

#include "Core/Logger/Logger.h"

#include "GUIExtensions.h"
#include "imfilebrowser.h"
#include <chrono>

#if OS_WINDOWS
#   include <Windows.h>
#   include <d3d12.h>
#   include <dxgi1_4.h>
#elif OS_LINUX
#   include <X11/Xlib.h>
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

    struct Frame
    {
        static Frame* Singleton;
        static Frame* GetSingleton();

        Frame();
        ~Frame();
        void Run();
        void PreInit();
        void PostInit();

        Frame* SetTitle(const char* T);
        const char* GetTitle() const;
    private:
        const char* Title = nullptr;
    public:

        // logging stuff
        ImGuiTextBuffer LogBuffer;
        Level CurrentLevel = Level::Info;
        void Log(Level L, const char* Msg);

        // all the gui stuff happens in here, is called once per frame
        void GUILoop();

        // this is called once to init all the gui data
        void InitGUI();

        // OS sepecfic stuff
#if OS_WINDOWS
        using FrameHandle = HWND;

        void SetChild(HWND C);
#elif OS_LINUX
        using FrameHandle = Window;
#elif OS_APPLE
        using FrameHandle = void*; // NSWindow*
#endif

        void FinalizePlayers();
        Array<


        // rendering stuff
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
        FrameHandle Handle;
    };
}