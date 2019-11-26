#include "pch.h"
#include "GamePanel.h"
#if __has_include("GamePanel.g.cpp")
#include "GamePanel.g.cpp"
#endif

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Media::Imaging;

namespace winrt::volts_gui::implementation
{
    GamePanel::GamePanel()
    {
        InitializeComponent();

        auto img = BitmapImage(Uri(L"E:\\Emu\\JJBA\\JJBA\\PS3_GAME\\ICON0.PNG"));
    }

    GamePanel::GamePanel(const hstring& title)
        : GamePanel()
    {
        TitleVal = title;
    }
}
