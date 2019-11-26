#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "GamesView.g.h"

namespace winrt::volts_gui::implementation
{
    struct GamesView : GamesViewT<GamesView>
    {
        GamesView();

        void ClickHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
    
        static const int card_width = 200;
        static const int card_height = 120;
    };
}

namespace winrt::volts_gui::factory_implementation
{
    struct GamesView : GamesViewT<GamesView, implementation::GamesView>
    {
    };
}
