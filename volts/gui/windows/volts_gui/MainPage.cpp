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

#include "../../../volt.h"

#include "GamesView.h"
#include "SettingsView.h"
#include "ManageView.h"

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
        view().Content(winrt::make<GamesView>());
    }

    void MainPage::NavigationChanged(NavigationView const& sender, NavigationViewItemInvokedEventArgs args)
    {
        if (args.IsSettingsInvoked())
        {
            // the settings tab is a special case
            view().Content(winrt::make<SettingsView>());
        }
        else
        {
            auto str = unbox_value<hstring>(args.InvokedItem());
            if (str == L"Games")
                view().Content(winrt::make<GamesView>());
            else if (str == L"Manage")
                view().Content(winrt::make<ManageView>());
        }
    }
}
