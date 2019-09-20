#include "Frame.h"

#include "imgui/imgui.h"

#include "Render/Render.h"

#include "Render/DX12/Render.h"

#if OS_WINDOWS
#   include "imgui/examples/imgui_impl_dx12.h"
#   include "imgui/examples/imgui_impl_win32.h"
#else
#   include <imgui/examples/imgui_impl_vulkan.h>
#endif

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

namespace Volts::GUI
{
#if OS_WINDOWS
    HINSTANCE Instance = {};

    Frame::Frame() {}

    Frame& Frame::Title(const String& T) { this->T = T; return *this; }

    LRESULT CALLBACK FrameProc(
        HWND Window,
        UINT Msg,
        WPARAM W,
        LPARAM L
    )
    {
        if(ImGui_ImplWin32_WndProcHandler(Window, Msg, W, L))
            return true;

        RSX::Render* Backend = reinterpret_cast<RSX::Render*>(GetWindowLongPtr(Window, GWLP_USERDATA));

        switch(Msg)
        {
            case WM_CREATE:
            {
                LPCREATESTRUCT Create = reinterpret_cast<LPCREATESTRUCT>(L);
                SetWindowLongPtr(Window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(Create->lpCreateParams));
            }
            return 0;

            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
        }

        return DefWindowProc(Window, Msg, W, L);
    }

    void Frame::Fullscreen()
    {
        CurrentRender->Fullscreen();
    }

    void Frame::Borderless()
    {
        CurrentRender->Borderless();
    }

    void Frame::Windowed()
    {
        CurrentRender->Windowed();
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
        WNDCLASSEX WC = {};
        WC.cbSize = sizeof(WNDCLASSEX);
        WC.style = CS_HREDRAW | CS_VREDRAW;
        WC.lpfnWndProc = FrameProc;
        WC.hInstance = Instance;
        WC.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
        WC.hCursor = LoadCursor(nullptr, IDC_ARROW);
        WC.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        WC.lpszClassName = T.CStr();
        WC.hIconSm = LoadIcon(nullptr, IDI_WINLOGO);

        RegisterClassEx(&WC);

        LONG Width = GetSystemMetrics(SM_CXSCREEN);
        LONG Height = GetSystemMetrics(SM_CYSCREEN);

        RECT Space = { 0L, 0L, Width, Height };

        AdjustWindowRectEx(
            &Space,
            WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
            false,
            WS_EX_APPWINDOW | WS_EX_WINDOWEDGE
        );

        Handle = CreateWindowEx(
            0,
            T.CStr(),
            T.CStr(),
            WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
            0,
            0,
            Space.right - Space.left,
            Space.bottom - Space.top,
            nullptr,
            nullptr,
            GUI::Instance,
            this
        );

        ShowWindow(Handle, SW_SHOW);
        SetForegroundWindow(Handle);
        SetFocus(Handle);

        // todo: lets not hardcode this shall we
        CurrentRender = new RSX::DX12();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        CurrentRender->Attach(this);

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

            Frame::GUILoop(this);

            CurrentRender->PresentRender();
        }

        CurrentRender->Detach();
        ImGui::DestroyContext();

        DestroyWindow(Handle);
        UnregisterClass(WC.lpszClassName, WC.hInstance);
    }
#else

#endif
}