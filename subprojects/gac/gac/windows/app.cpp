#include "app.h"

using namespace winrt;

using namespace Windows::UI::Xaml::Hosting;

namespace gac
{
    LRESULT CALLBACK proc(HWND wnd, UINT code, WPARAM wparam, LPARAM lparam)
    {
        switch(code)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_CREATE:
            //SetWindowLongPtrA(wnd, GWLP_USERDATA, (LONG_PTR)reinterpret_cast<app*>(((LPCREATESTRUCT)lparam)->lpCreateParams));
            break;
        default:
            return DefWindowProc(wnd, code, wparam, lparam);
        }

        return DefWindowProc(wnd, code, wparam, lparam);
    }

    app::app(const char* title, uint32_t width, uint32_t height)
    {
        inst = GetModuleHandle(nullptr);

        WNDCLASSEX cls = { };
        
        auto len = strlen(title);

        wchar_t* wtitle = new wchar_t[len];
        MultiByteToWideChar(0, 0, title, len, wtitle, len);

        cls.cbSize = sizeof(WNDCLASSEX);
        cls.lpfnWndProc = gac::proc;
        cls.hInstance = inst;
        cls.lpszClassName = title;
        cls.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

        cls.hIconSm = LoadIcon(cls.hInstance, IDI_APPLICATION);

        if(!RegisterClassEx(&cls))
        {
            MessageBox(nullptr, "failed to register class", "error", 0);
            std::abort();
        }

        wnd = CreateWindow(
            title,
            title,
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT, width, height,
            nullptr,
            nullptr,
            inst,
            this
        );

        if(!wnd)
        {
            MessageBox(nullptr, "failed to create window", "error", 0);
            std::abort();
        }

        MessageBox(nullptr, "here1", "error", 0);

        winrt::init_apartment(apartment_type::single_threaded);

        MessageBox(nullptr, "here2", "error", 0);

        auto manager = WindowsXamlManager::InitializeForCurrentThread();
    
        MessageBox(nullptr, "here10", "error", 0);

        DesktopWindowXamlSource source;

        MessageBox(nullptr, "here11", "error", 0);

        auto interop = source.as<IDesktopWindowXamlSourceNative>();

        MessageBox(nullptr, "here12", "error", 0);

        // TODO: check error
        winrt::check_hresult(interop->AttachToWindow(wnd));

        MessageBox(nullptr, "here5", "error", 0);

        HWND island = nullptr;
        interop->get_WindowHandle(&island);
        SetWindowPos(island, 0, 0, 0, width, height, SWP_SHOWWINDOW);

        MessageBox(nullptr, "here6", "error", 0);

        Windows::UI::Xaml::Controls::Grid container;
        
        Windows::UI::Xaml::Controls::TextBlock text;
        text.Text(L"hello");
        text.FontSize(48);
        
        container.Children().Append(text);
        container.UpdateLayout();
        source.Content(container);

        MessageBox(nullptr, "here4", "error", 0);

        // TODO: we may need to get data from wWinMain
        ShowWindow(wnd, SW_SHOW);
        UpdateWindow(wnd);

        MessageBox(nullptr, "here3", "error", 0);
    }

    void app::run()
    {
        MSG msg = {};
        while(GetMessage(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}