#pragma once

#include "pch.h"

#include "item.h"

#include "simple.h"

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
        nav(std::initializer_list<navitem> nitems)
        {
            for(auto i : nitems)
            {
                view.MenuItems().Append(i.item);
                items.push_back(i);
            }

            view.Content(content);
            content.Content(text("aaaa"));

            view.ItemInvoked([this](auto const& sender, auto const& args) {
                //content.Navigate(
                //    winrt::xaml_typename<temp>(), 
                //    nullptr
                //);
            });

            view.UpdateLayout();
        }

        nav& align(align a)
        {
            view.PaneDisplayMode((winrt::Windows::UI::Xaml::Controls::NavigationViewPaneDisplayMode)a);
            return *this;
        }

        std::vector<navitem> items;

        winrt::Windows::UI::Xaml::Controls::Frame content;

        winrt::Windows::UI::Xaml::Controls::NavigationView view;
    };
}
