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
        Button B{nullptr};

        void OnLaunched(LaunchActivatedEventArgs const& Args)
        {       
            B = Button();     
            B.Content(box_value(L"Name jeff"));

            B.Click([](auto const&, auto const&) {
                MessageDialog D(L"AAAA");
                D.ShowAsync();
            });

            auto W = Window::Current();
            W.Content(B);
            W.Activate();
        }

        static void Run()
        {
            Application::Start([](auto&&){ make<GUI>(); });
        }
    };
}