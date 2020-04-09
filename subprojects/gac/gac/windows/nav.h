#pragma once

#include "pch.h"

#include "item.h"

#include "simple.h"

#include "horror.h"

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
                //MessageBoxW(nullptr, L"arse", L"arse", 0);
                //Windows::UI::Xaml::Interop::TypeName type;
                //type.Name = L"gac.GPage";
                //type.Kind = Windows::UI::Xaml::Interop::TypeKind::Custom;
                //content.Navigate(
                //    type,
                //    text("jeff")
                //);
                //winrt::Windows::UI::Xaml::Controls::Page p;
                //p.Content(text("ARSE"));
                //content.Content(p);
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
