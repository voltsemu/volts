#pragma once

#include "Meta/Macros.h"

#if OS_WINDOWS
#   define _UNICODE
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

        void Open();
        void Close();

        Size GetSize() const;

        operator WindowHandle() const { return Handle; }

#if OS_WINDOWS
        HINSTANCE Instance;
    private:
        WNDCLASSEXW WC;
#endif

    private:
        void CreateFrame();
        WindowHandle Handle;
    };
}