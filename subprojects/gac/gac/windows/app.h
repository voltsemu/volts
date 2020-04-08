#pragma once

#include "pch.h"

namespace gac
{
    struct app
    {
        app(const char* title, uint32_t width, uint32_t height);

        void run();

    private:
        HINSTANCE inst;
        HWND wnd;
    };
}

#define GAC_MAIN(...) int WINAPI wWinMain(HINSTANCE inst, HINSTANCE prev, PWSTR cmd, int show) { __VA_ARGS__; }