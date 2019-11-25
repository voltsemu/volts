#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"

using namespace winrt;
using namespace Windows;
using namespace Foundation;
using namespace UI;
using namespace Popups;
using namespace Xaml;
using namespace Controls;

namespace winrt::volts_gui::implementation
{
    MainPage::MainPage()
    {
        InitializeComponent();
    }

    void MainPage::NavigationLoaded(const IInspectable& sender, RoutedEventArgs args)
    {
        // default to the games page
        // TODO: make this configurable in settings
        set_page(L"Games");
    }

    void MainPage::NavigationChanged(NavigationView const& sender, NavigationViewItemInvokedEventArgs args)
    {
        if (args.IsSettingsInvoked())
        {
            // the settings tab is a special case
            set_page(L"Settings");
        }
        else
        {
            // everything else is just a string
            auto content = pages.at(unbox_value<hstring>(args.InvokedItem()));
            content_grid().Children().Append(content.get_content());
        }
    }

    void MainPage::set_page(const winrt::hstring& name)
    {
        // clear the grid
        content_grid().Children().Clear();
        // get the correct page
        auto content = pages.at(name);
        // then add that page to the now empty grid
        content_grid().Children().Append(content.get_content());
    }
}
