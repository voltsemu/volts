#pragma once

#include "PCH.h"

namespace Volts
{
    using namespace winrt;
    using namespace Windows::ApplicationModel;
    using namespace Windows::ApplicationModel::Activation;
    using namespace Windows::Foundation;
    
    using namespace Windows::UI;
    using namespace Windows::UI::Xaml;
    using namespace Windows::UI::Xaml::Controls;
    using namespace Windows::UI::Xaml::Controls::Primitives;
    using namespace Windows::UI::Xaml::Interop;
    using namespace Windows::UI::Xaml::Media;
    using namespace Windows::UI::Xaml::Navigation;
    using namespace Windows::UI::Popups;

    struct GUI : ApplicationT<GUI>
    {
        GUI() = default;

        static void Clicked(IInspectable const& Sender, const RoutedEventArgs& Args)
        {
            MessageDialog D(L"You pressed the button");
            D.ShowAsync();
        }

        static void OnLaunched(LaunchActivatedEventArgs const& Args)
        {            
            Button B;
            IInspectable Caption = PropertyValue::CreateString(L"Name jeff");
            B.Content(Caption);

            B.Background(SolidColorBrush(Colors::Red()));
            B.Foreground(SolidColorBrush(Colors::Yellow()));

            B.Click(&Clicked);

            auto W = Window::Current();
            W.Content(B);
            W.Activate();
        }

        static void Run()
        {
            winrt::init_apartment();
            Application::Start([](auto&&){ make<GUI>(); });
        }
    };
}