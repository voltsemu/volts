#include "GUI.h"

#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.UI.Core.h>

using namespace winrt;
using namespace winrt::Windows::ApplicationModel::Core;
using namespace winrt::Windows::UI::Core;

namespace Volts::PS3
{
    struct App : IFrameworkViewSource
    {
        IFrameworkView CreateView()
        {
            return IFrameworkView();
        }
    };

    GUI::GUI()
    {
        CoreApplication::Run(App());
    }
}