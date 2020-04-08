#pragma once

#include "pch.h"

namespace gac
{
    using navitem = winrt::Windows::UI::Xaml::Controls::NavigationViewItem;

    navitem item(const char* label);
}