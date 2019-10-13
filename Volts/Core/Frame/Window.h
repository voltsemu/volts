#pragma once

#include "Meta/Macros.h"

#if OS_WINDOWS
#   include <windows.h>
#endif

namespace Volts
{
#if OS_WINDOWS
    using WindowHandle = HWND;
#elif OS_LINUX
    using WindowHandle = Window;
#else
    using WindowHandle = void*; // NSWindow*
#endif
    struct Window
    {
        Window();
        ~Window();

        void Show();

        Size GetSize() const;

        operator WindowHandle() const { return Handle; }
    private:
        void CreateFrame();
        WindowHandle Handle;
    };
}