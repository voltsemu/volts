#pragma once

#include "pch.h"

#include "item.h"

namespace gac
{
    enum class align : int32_t
    {
        none = (int32_t)winrt::Windows::UI::Xaml::Controls::NavigationViewPaneDisplayMode::Auto,
        left = (int32_t)winrt::Windows::UI::Xaml::Controls::NavigationViewPaneDisplayMode::Left,
        top = (int32_t)winrt::Windows::UI::Xaml::Controls::NavigationViewPaneDisplayMode::Top
    };

    struct nav
    {
        nav(std::initializer_list<navitem> items)
        {
            for(auto i : items)
                view.MenuItems().Append(i);
        }

        nav& align(align a)
        {
            view.PaneDisplayMode((winrt::Windows::UI::Xaml::Controls::NavigationViewPaneDisplayMode)a);
            return *this;
        }

        winrt::Windows::UI::Xaml::Controls::NavigationView view;
    };
}