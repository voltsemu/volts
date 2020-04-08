#pragma once

#include "pch.h"

namespace gac
{
    using namespace winrt::Windows::UI::Xaml;


    struct navitem
    {
        navitem() = delete;

        template<typename T>
        navitem& destination(T val)
        {
            //type = winrt::xaml_typename<T>();
            //dest = val;
            return *this;
        }

        winrt::Windows::UI::Xaml::Controls::NavigationViewItem item;

        //winrt::Windows::UI::Xaml::Interop::TypeName type;
        //winrt::Windows::UI::Xaml::UIElement dest = nullptr;
    };

    namespace icon
    {
        winrt::Windows::UI::Xaml::Controls::FontIcon game();
        winrt::Windows::UI::Xaml::Controls::FontIcon view();
        winrt::Windows::UI::Xaml::Controls::FontIcon files();
        winrt::Windows::UI::Xaml::Controls::FontIcon tools();
    }

    navitem item(const char* label);

    template<typename T>
    navitem item(const char* label, T icn)
    {
        auto i = item(label);
        i.item.Icon(icn);
        return i;
    }
}