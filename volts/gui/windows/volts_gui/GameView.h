#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "GameView.g.h"

namespace winrt::volts_gui::implementation
{
    struct GameView : GameViewT<GameView>
    {
        GameView();
    };
}

namespace winrt::volts_gui::factory_implementation
{
    struct GameView : GameViewT<GameView, implementation::GameView>
    {
    };
}
