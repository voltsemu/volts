#pragma once

#include "pch.h"

#include "nav.h"

namespace gac
{
    struct app
    {
        app(const char* title, uint32_t width, uint32_t height);

        void run();

        app& operator-(const nav& view)
        {
            view.view.UpdateLayout();
            source.Content(view.view);
            return *this;
        }

        winrt::Windows::UI::Xaml::Hosting::DesktopWindowXamlSource source;
        winrt::Windows::UI::Xaml::Hosting::WindowsXamlManager manager = nullptr;
        HINSTANCE inst;
        HWND wnd;
        HWND island;
    };
}

#define GAC_MAIN(...) int WINAPI wWinMain(HINSTANCE inst, HINSTANCE prev, PWSTR cmd, int show) { __VA_ARGS__; }