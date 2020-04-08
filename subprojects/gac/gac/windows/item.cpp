#pragma once

#include "item.h"

#define GLYPH(str) [] { winrt::Windows::UI::Xaml::Controls::FontIcon fi;             fi.FontFamily(winrt::Windows::UI::Xaml::Media::FontFamily(winrt::to_hstring("Segoe MDL2 Assets"))); fi.Glyph(winrt::to_hstring(str)); return fi; }();

namespace gac
{   
    namespace icon
    {
        winrt::Windows::UI::Xaml::Controls::FontIcon game()
        {
            return GLYPH(L"\xE7FC");
        }

        winrt::Windows::UI::Xaml::Controls::FontIcon view()
        {
            return GLYPH(L"\xE7C4");
        }

        winrt::Windows::UI::Xaml::Controls::FontIcon files()
        {
            return GLYPH(L"\xEC50");
        }

        winrt::Windows::UI::Xaml::Controls::FontIcon tools()
        {
            return GLYPH(L"\xEC7A");
        }
    }

    navitem item(const char* label)
    {
        winrt::Windows::UI::Xaml::Controls::NavigationViewItem i;
        auto name = winrt::box_value(winrt::to_hstring(label));
        i.Content(name);
        i.Tag(name);

        navitem out = { i };

        return out;
    }
}