#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include <winrt/Windows.UI.Input.Inking.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include "MainUserControl.g.h"

namespace winrt::VoltsGUI::implementation
{
    struct MainUserControl : MainUserControlT<MainUserControl>
    {
        MainUserControl();
    };
}

namespace winrt::VoltsGUI::factory_implementation
{
    struct MainUserControl : MainUserControlT<MainUserControl, implementation::MainUserControl>
    {
    };
}
