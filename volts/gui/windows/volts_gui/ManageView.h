#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "ManageView.g.h"

namespace winrt::volts_gui::implementation
{
    struct ManageView : ManageViewT<ManageView>
    {
        ManageView();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void ClickHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::volts_gui::factory_implementation
{
    struct ManageView : ManageViewT<ManageView, implementation::ManageView>
    {
    };
}
