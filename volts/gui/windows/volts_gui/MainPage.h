#pragma once

#include "MainPage.g.h"

namespace winrt::volts_gui::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();

        void NavigationLoaded(const winrt::Windows::Foundation::IInspectable& sender, winrt::Windows::UI::Xaml::RoutedEventArgs args);

        void NavigationChanged(winrt::Windows::UI::Xaml::Controls::NavigationView const& sender, winrt::Windows::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs args);
    };
}

namespace winrt::volts_gui::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
