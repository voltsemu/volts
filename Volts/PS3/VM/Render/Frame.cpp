#include "Frame.h"

namespace Volts::PS3::RSX
{
    void Frame::SetHeight(unsigned H)
    {
        Height = H;
    }

    void Frame::SetWidth(unsigned W)
    {
        Width = W;
    }

    void Frame::SetX(unsigned XIn)
    {
        X = XIn;
    }

    void Frame::SetY(unsigned YIn)
    {
        Y = YIn;
    }

    void Frame::SetTitle(const char* T)
    {
        Title = T;
    }

    void Create()
    {
#if OS_WINDOWS
        
#elif OS_LINUX
        D = XOpenDisplay(nullptr);

        // TODO: figure out how to log here
        if(D == nullptr)
            return;

        int S = DefaultScreen(D);

        Handle = XCreateSimpleWindow(
            D,
            RootWindow(D, S),
            X, Y,
            Width, Height,
            5, BlackPixel(D, S), WhitePixel(D, S)
        );
#elif OS_APPLE

#endif
    }

    void Frame::Close()
    {
#if OS_WINDOWS

#elif OS_LINUX
        XDestroyWindow(D, Handle);
        XCloseDisplay(D);
#elif OS_APPLE

#endif
    }

    FrameHandle Frame::GetHandle() const
    {
        return Handle;
    }
}