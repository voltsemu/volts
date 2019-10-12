#pragma once

#include <Meta/Aliases.h>
#include <Meta/Macros.h>

#include "GUIExtensions.h"

#define OBJC_CLASS(Name, Super) @interface Name : Super @end @implementation Name

namespace Volts::GUI
{
    using namespace Cthulhu;

#if OS_WINDOWS
    using FrameHandle = HWND;
    extern HINSTANCE Instance;
#elif OS_APPLE
    using FrameHandle = void*;
#else
    using FrameHandle = Window;
#endif

    struct Size
    {
        U32 Width, Height;
    };

    struct Frame
    {
        Frame();
        ~Frame();

        void PreInit();
        void PostInit();

        Frame& SetTitle(const char* NewTitle);
        void Run();

#if OS_WINDOWS
        void SetChild(HWND Window);
#endif

        Size GetSize() const;

        FrameHandle Handle;
        const char* Title = nullptr;
    };
}
