#pragma once

#include "MainPage.g.h"

struct ContentPage
{
    virtual winrt::Windows::UI::Xaml::UIElement get_content() const 
    {
        return winrt::Windows::UI::Xaml::Controls::Grid();
    }
};

struct GamesPage : ContentPage
{
    virtual winrt::Windows::UI::Xaml::UIElement get_content() const override
    {
        return winrt::Windows::UI::Xaml::Controls::Grid();
    }
};

struct ManagePage : ContentPage
{
    virtual winrt::Windows::UI::Xaml::UIElement get_content() const override
    {
        return winrt::Windows::UI::Xaml::Controls::Grid();
    }
};

struct SettingsPage : ContentPage
{
    virtual winrt::Windows::UI::Xaml::UIElement get_content() const override
    {
        return winrt::Windows::UI::Xaml::Controls::Grid();
    }
};

namespace winrt::volts_gui::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();

        void NavigationLoaded(const winrt::Windows::Foundation::IInspectable& sender, winrt::Windows::UI::Xaml::RoutedEventArgs args);

        void NavigationChanged(winrt::Windows::UI::Xaml::Controls::NavigationView const& sender, winrt::Windows::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs args);

        void set_page(const winrt::hstring& name);

        const std::map<hstring, ContentPage> pages = {
            { L"Games", GamesPage() },
            { L"Manage", ManagePage() },
            { L"Settings", SettingsPage() }
        };
    };
}

namespace winrt::volts_gui::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
