#include "pch.h"
#include "ManageView.h"
#if __has_include("ManageView.g.cpp")
#include "ManageView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::volts_gui::implementation
{
    ManageView::ManageView()
    {
        InitializeComponent();
    }

    int32_t ManageView::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void ManageView::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void ManageView::ClickHandler(IInspectable const&, RoutedEventArgs const&)
    {
        Button().Content(box_value(L"Clicked"));
    }
}
