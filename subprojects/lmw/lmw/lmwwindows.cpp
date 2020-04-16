#include "lmwwindows.h"

#include <stdint.h>
#include <Windows.h>

#include <stdio.h>

namespace lmw
{
    LRESULT CALLBACK WindowProc(HWND wnd, UINT msg, WPARAM w, LPARAM l)
    {
        switch(msg)
        {
        case WM_CLOSE:
            DestroyWindow(wnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProcW(wnd, msg, w, l);
    }

    window::window(const char* title, uint32_t width, uint32_t height)
    {
        wchar_t str[2048] = {};
        swprintf_s(str, L"%p", title);

        const wchar_t* cls = str;

        WNDCLASSW wc = {};
        wc.lpfnWndProc = WindowProc;
        wc.lpszClassName = cls;
        wc.hInstance = GetModuleHandleW(nullptr);

        RegisterClassW(&wc);

        win = CreateWindowExW(0, cls, str,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, width, height,
            nullptr,
            nullptr,
            wc.hInstance,
            nullptr
        );

        ShowWindow(win, SW_SHOW);
    }

    std::tuple<uint32_t, uint32_t> window::size() const
    {
        RECT rect;
        GetWindowRect(win, &rect);
        return std::make_tuple(rect.right - rect.left, rect.bottom - rect.top);
    }

    void window::close()
    {
        DestroyWindow(win);
    }

    bool window::poll()
    {
        MSG msg = {};
        if(GetMessage(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            return true;
        }

        return false;
    }

    void window::rename(const char* title)
    {
        wchar_t str[2048] = {};
        swprintf_s(str, L"%p", title);
        SetWindowTextW(win, str);
    }

    HWND window::handle() const { return win; }
    
    const char* ext[] = {
        "VK_KHR_surface",
        "VK_KHR_win32_surface"
    };
    
    const char** vk_extensions(uint32_t* num)
    {
        *num = sizeof(ext) / sizeof(const char*);
        return ext;
    }
}