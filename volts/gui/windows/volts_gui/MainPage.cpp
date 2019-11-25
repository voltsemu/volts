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
        set_page(L"Games");
    }

    void MainPage::NavigationChanged(NavigationView const& sender, NavigationViewItemInvokedEventArgs args)
    {
        if (args.IsSettingsInvoked())
        {
            set_page(L"Settings");
        }
        else
        {
            auto content = pages.at(unbox_value<hstring>(args.InvokedItem()));
            content_grid().Children().Append(content.get_content());
        }
    }

    void MainPage::set_page(const winrt::hstring& name)
    {
        content_grid().Children().Clear();
        auto content = pages.at(name);

        content_grid().Children().Append(content.get_content());
    }
}
