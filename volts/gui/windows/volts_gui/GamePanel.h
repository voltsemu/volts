#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "GamePanel.g.h"

namespace winrt::volts_gui::implementation
{
    struct GamePanel : GamePanelT<GamePanel>
    {
        GamePanel();
        GamePanel(const hstring& title);

        hstring Title() { return TitleVal; }
        void Title(hstring in)
        {
            TitleVal = in;
        }

    private:
        hstring TitleVal;
    };
}

namespace winrt::volts_gui::factory_implementation
{
    struct GamePanel : GamePanelT<GamePanel, implementation::GamePanel>
    {
    };
}
