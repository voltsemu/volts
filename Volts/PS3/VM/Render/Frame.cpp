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

#if OS_WINDOWS
    void Frame::Create(Render* Parent)
#else
    void Frame::Create()
#endif
    {
#if OS_WINDOWS
        const char WindowClassName[] = "VoltsFrameClass";
        WNDCLASSEX WC = { 0 };
        WC.cbSize = sizeof(WNDCLASSEX);
        WC.style = CS_HREDRAW | CS_VREDRAW;
        WC.lpfnWndProc = InputCallback;
        WC.hInstance = Instance;
        WC.hCursor = LoadCursor(nullptr, IDC_ARROW);
        WC.lpszClassName = WindowClassName;
        WC.cbClsExtra = sizeof(Render*);
        RegisterClassEx(&WC);

        Handle = CreateWindowExW(
            0,
            std::wstring(&WindowClassName[0], &WindowClassName[sizeof(WindowClassName)]).c_str(),
            std::wstring(&Title[0], &Title[strlen(Title)]).c_str(),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            Width,
            Height,
            nullptr,
            nullptr,
            Instance,
            Parent
        );

        if(Handle == nullptr)
        {
            // figure out how to log
            return;
        }

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
        // TODO: apple support is going to be painful...
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

    void Frame::Show()
    {
#if OS_WINDOWS
        ShowWindow(Handle, CmdShow);
#elif OS_LINUX
        // TODO: all this
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