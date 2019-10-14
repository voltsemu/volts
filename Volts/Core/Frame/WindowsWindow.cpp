#include "Window.h"

namespace Volts
{
    LRESULT CALLBACK FrameProc(
        HWND Window,
        UINT Msg,
        WPARAM W,
        LPARAM L
    )
    {
        if(Msg == WM_DESTROY)
        {
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProcW(Window, Msg, W, L);
    }

    void Window::Open()
    {
        ShowWindow(Handle, SW_SHOW);
        UpdateWindow(Handle);
    }

    void Window::Close()
    {
        DestroyWindow(Handle);
        UnregisterClassW(WC.lpszClassName, Instance);
    }

    Size Window::GetSize() const
    {
        return { 0, 0 };
    }

    void Window::CreateFrame()
    {
        WC = { sizeof(WC) };
        WC.style = CS_HREDRAW | CS_VREDRAW;
        WC.lpfnWndProc = FrameProc;
        WC.hInstance = Instance;
        WC.hCursor = LoadCursor(nullptr, IDC_ARROW);
        WC.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        WC.lpszClassName = L"VoltsWindowClass";

        RegisterClassExW(&WC);

        Handle = CreateWindowExW(
            0,
            WC.lpszClassName,
            L"Volts",
            WS_OVERLAPPEDWINDOW,
            0, 0,
            CW_USEDEFAULT, CW_USEDEFAULT,
            nullptr,
            nullptr,
            Instance,
            nullptr
        );
    }
}