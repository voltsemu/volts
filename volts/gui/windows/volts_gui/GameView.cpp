#include "pch.h"
#include "GameView.h"
#if __has_include("GameView.g.cpp")
#include "GameView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::volts_gui::implementation
{
    GameView::GameView()
    {
        InitializeComponent();
    }
}
