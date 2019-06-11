#include "Frame.h"

namespace Volts::PS3::RSX
{
#if OS_WINDOWS
    int CmdShow = 0;
    HINSTANCE Instance = nullptr;

    void Frame::InputHandle(WindowCallback Callback)
    {
        InputCallback = Callback;
    }
#endif

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

    void Frame::Create()
    {
#if OS_WINDOWS
        const char* WindowClassName = "VoltsFrameClass";
        WNDCLASS WC = {};
        WC.lpfnWndProc = InputCallback;
        WC.hInstance = Instance;
        WC.lpszClassName = WindowClassName;

        RegisterClass(&WC);

        Handle = CreateWindowEx(
            0,
            WindowClassName,
            Title,
            WS_OVERLAPPEDWINDOW,

            X, Y, Width, Height,

            nullptr,
            nullptr,
            Instance,
            nullptr
        );

        if(Handle == nullptr)
        {
            // figure out how to log
            return;
        }

        ShowWindow(Handle, CmdShow);
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
        CloseWindow(Handle);
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

#if OS_WINDOWS
    bool Frame::ShouldStayOpen()
    {
        // the hi word of GetQueueStatus is the amount of messages matching QS_ALLINPUT
        // and since QS_ALLINPUT matches everything it returns the amount of messages
        // in the queue
        return GetMessage(&LastMessage, nullptr, 0, 0) != -1;
    }

    void Frame::PollEvents()
    {
        TranslateMessage(&LastMessage);
        DispatchMessage(&LastMessage);
    }
#endif
}