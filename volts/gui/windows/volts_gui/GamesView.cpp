#include "pch.h"
#include "GamesView.h"
#if __has_include("GamesView.g.cpp")
#include "GamesView.g.cpp"
#endif

#include "GamePanel.h"

using namespace winrt;
using namespace Windows::UI::Popups;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Controls;

namespace winrt::volts_gui::implementation
{
    GamesView::GamesView()
    {
        InitializeComponent();

        std::vector<hstring> games = { L"game", L"aaaa", L"bbbb" };

        for (hstring& h : games)
        {
            panel().Children().Append(winrt::make<GamePanel>(h));
        }
    }

    void GamesView::ClickHandler(IInspectable const&, RoutedEventArgs const&)
    {
        Button().Content(box_value(L"Clicked"));
    }
}
