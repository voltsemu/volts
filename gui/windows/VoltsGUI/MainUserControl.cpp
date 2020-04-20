#include "pch.h"
#include "MainUserControl.h"
#if __has_include("MainUserControl.g.cpp")
#include "MainUserControl.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::VoltsGUI::implementation
{
    MainUserControl::MainUserControl()
    {
        InitializeComponent();
    }
}
