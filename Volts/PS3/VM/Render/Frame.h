#pragma once

// basically the only header we can have here because objective C++
#include <Meta/Macros.h>

#if OS_WINDOWS
#   include <Windows.h>
#elif OS_LINUX
#   include <X11/Xlib.h>
#   include <X11/Xos.h>
#   include <X11/Xutil.h>
#endif

namespace Volts::PS3::RSX
{

#if OS_WINDOWS
    using FrameHandle = HWND;
#elif OS_LINUX
    using FrameHandle = Window;
#elif OS_APPLE
    using FrameHandle = void*; // NSView*
#endif

    struct Frame
    {
        void SetHeight(unsigned H);
        void SetWidth(unsigned W);
        void SetX(unsigned X);
        void SetY(unsigned Y);
        void SetTitle(const char* T);
        void Create();
        void Close();
        FrameHandle GetHandle() const;

    private:
        unsigned Width, Height;
        unsigned X, Y;
        const char* Title;
        FrameHandle Handle;
#if OS_LINUX
        Display* D;
#endif
    };
}