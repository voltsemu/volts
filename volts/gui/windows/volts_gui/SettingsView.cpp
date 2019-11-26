#include "pch.h"
#include "SettingsView.h"
#if __has_include("SettingsView.g.cpp")
#include "SettingsView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::volts_gui::implementation
{
    SettingsView::SettingsView()
    {
        InitializeComponent();
    }

    int32_t SettingsView::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void SettingsView::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void SettingsView::ClickHandler(IInspectable const&, RoutedEventArgs const&)
    {
        Button().Content(box_value(L"Clicked"));
    }
}
