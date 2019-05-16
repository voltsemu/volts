#pragma once

#include "Meta/Aliases.h"

#if OS_WINDOWS
#   include <Windows.h>
#elif OS_LINUX
#   include <X11/X.h>
#endif

namespace Volts
{
    struct Window
    {
        Window();
        Cthulhu::U32 Width() const;
        Cthulhu::U32 Height() const;

        Cthulhu::U32 X() const;
        Cthulhu::U32 Y() const;

        using WindowHandle = 
#if OS_WINDOWS
    HWND;
#elif OS_LINUX
    Window;
#elif OS_APPLE
    void*; //NSWindow
#endif

        WindowHandle* GetHandle() const;
    private:
        WindowHandle Handle;
    };
}