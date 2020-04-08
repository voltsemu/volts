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

        winrt::init_apartment(apartment_type::single_threaded);

        auto manager = WindowsXamlManager::InitializeForCurrentThread();
    
        DesktopWindowXamlSource source;

        auto interop = source.as<IDesktopWindowXamlSourceNative>();

        winrt::check_hresult(interop->AttachToWindow(wnd));

        HWND island = nullptr;
        interop->get_WindowHandle(&island);
        SetWindowPos(island, 0, 0, 0, width, height, SWP_SHOWWINDOW);

        Windows::UI::Xaml::Controls::StackPanel container;
        container.Background(Windows::UI::Xaml::Media::SolidColorBrush{ Windows::UI::Colors::LightGray() });
        
        Windows::UI::Xaml::Controls::TextBlock text;
        text.Text(L"hello");
        text.VerticalAlignment(Windows::UI::Xaml::VerticalAlignment::Center);
	    text.HorizontalAlignment(Windows::UI::Xaml::HorizontalAlignment::Center);
        text.FontSize(48);
        
        container.Children().Append(text);
        container.UpdateLayout();

        source.Content(container);

        // TODO: we may need to get data from wWinMain
        ShowWindow(wnd, SW_SHOW);
        UpdateWindow(wnd);

        MSG msg = {};
        while(GetMessage(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
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