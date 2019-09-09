#include "GUI.h"

#include "Volts/PS3/VM/Render/Frame.h"

namespace Volts::PS3
{
    using namespace winrt;
    using namespace Windows::UI;
    using namespace Windows::UI::Xaml;
    using namespace Windows::UI::Xaml::Media;
    using namespace Windows::UI::Xaml::Controls;
    using namespace Windows::UI::Xaml::Hosting;

    GUI::GUI()
    {
        // create the win32 window
        WNDCLASSEX WC = {};

        const wchar_t WindowClass[] = L"VoltsGUIClass";

        WC.cbSize = sizeof(WNDCLASSEX);
        WC.style = CS_HREDRAW | CS_VREDRAW;
        WC.lpfnWndProc = DefWindowProcW;
        WC.lpszClassName = WindowClass;
        WC.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

        RegisterClassEx(&WC);
        HWND Window = CreateWindowW(
            WindowClass,
            L"Volts",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            nullptr,
            nullptr,
            RSX::Instance,
            nullptr
        );

        // lets rev up those fryers
        init_apartment(apartment_type::single_threaded);

        WindowsXamlManager::InitializeForCurrentThread();

        DesktopWindowXamlSource Source;
        auto Interop = Source.as<IDesktopWindowXamlSourceNative>();
        Interop->AttachToWindow(Window);

        HWND Island = nullptr;
        Interop->get_WindowHandle(&Island);
        SetWindowPos(Island, 0, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, SWP_SHOWWINDOW);

        StackPanel Panel;
        Panel.Background(SolidColorBrush{Colors::LightGray()});

        TextBlock Text;
        Text.Text(L"My name jeff");
        Text.VerticalAlignment(VerticalAlignment::Center);
        Text.HorizontalAlignment(HorizontalAlignment::Center);
        Text.FontSize(48);

        Panel.Children().Append(Text);
        Panel.UpdateLayout();
        Source.Content(Panel);

        // show the window and do the event loop
        ShowWindow(Window, RSX::CmdShow);
        UpdateWindow(Window);

        MSG Message = { };
        while (GetMessage(&Message, NULL, 0, 0))
        {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }
    }
}