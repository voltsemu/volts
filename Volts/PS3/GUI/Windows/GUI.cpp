#include "GUI.h"

#include <winrt/Windows.Foundation.h>

namespace Volts::PS3
{
    GUI::GUI()
    {
        winrt::init_apartment();
    }
}