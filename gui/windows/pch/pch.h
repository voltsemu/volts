#define UNICODE
#define _WINDOWS

#pragma comment(lib, "windowsapp")

// winrt is a bit noisy
#pragma warning(push)
#pragma warning(disable : 4002)
#pragma warning(disable : 4715)

#include <Windows.h>

#include <stdlib.h>
#include <string.h>

#include <winrt/base.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Popups.h>
#include <winrt/Windows.Foundation.Collections.h>

#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>

#pragma warning(pop)