#include "Frame.h"

#include "imgui/imgui.h"

#include "Render/Render.h"

#include "Render/DX12/Render.h"
#include "imgui/examples/imgui_impl_win32.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

namespace Volts::GUI
{
    HINSTANCE Instance = {};
    HWND Child = {};

    void Frame::SetChild(HWND C)
    {
        Child = C;
    }

    LRESULT CALLBACK FrameProc(
        HWND Window,
        UINT Msg,
        WPARAM W,
        LPARAM L
    )
    {
        switch(Msg)
        {
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
        }
        return DefWindowProc(Window, Msg, W, L);
    }

    Size Frame::GetSize() const
    {
        RECT Rect;
        GetWindowRect(Handle, &Rect);
        return {
            static_cast<U32>(Rect.right - Rect.left),
            static_cast<U32>(Rect.bottom - Rect.top)
        };
    }

    void Frame::Run()
    {
        FinalizeRenders();
        WNDCLASSEX WC = { sizeof(WNDCLASSEX) };
        WC.style = CS_HREDRAW | CS_VREDRAW;
        WC.lpfnWndProc = FrameProc;
        WC.hInstance = GUI::Instance;
        WC.hCursor = LoadCursor(nullptr, IDC_ARROW);
        WC.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        WC.lpszClassName = "VoltsWindowClass";

        RegisterClassEx(&WC);

        Handle = CreateWindowEx(
            0,
            "VoltsWindowClass",
            "Volts",
            WS_OVERLAPPEDWINDOW,
            0, 0,
            CW_USEDEFAULT, CW_USEDEFAULT,
            nullptr,
            nullptr,
            GUI::Instance,
            nullptr
        );

        ShowWindow(Handle, SW_SHOW);
        UpdateWindow(Handle);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplWin32_Init(Handle);

        SetRender("DX12");
        UpdateDevices();

        MSG Message = {};
        while(Message.message != WM_QUIT)
        {
            if(PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&Message);
                DispatchMessage(&Message);
            }

            CurrentRender->BeginRender();

            ImGui::NewFrame();
            GUILoop();
            ImGui::Render();

            CurrentRender->PresentRender();
        }

        ImGui_ImplWin32_Shutdown();

        DestroyWindow(Handle);
        UnregisterClass(WC.lpszClassName, WC.hInstance);
    }
}