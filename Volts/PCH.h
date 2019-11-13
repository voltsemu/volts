#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#define CXXOPTS_NO_EXCEPTIONS
#define CXXOPTS_NO_RTTI

#include "cxxopts/cxxopts.h"

#define FMT_EXCEPTIONS 0
#include <fmt/format.h>

#include <Meta/Macros.h>
#include <Meta/Aliases.h>

#if OS_WINDOWS
#   pragma comment(lib, "windowsapp")
#   pragma warning(push)
#   pragma warning(disable : 4002)
#   include <winrt/base.h>
#   include <winrt/Windows.ApplicationModel.h>
#   include <winrt/Windows.ApplicationModel.Activation.h>
#   include <winrt/Windows.Foundation.h>
#   include <winrt/Windows.Foundation.Collections.h>
#   include <winrt/Windows.UI.Xaml.h>
#   include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#   include <winrt/Windows.UI.Xaml.Controls.h>
#   include <winrt/Windows.UI.Xaml.Media.h>
#   include <winrt/Windows.UI.h>
#   include <winrt/Windows.UI.Popups.h>
#   pragma warning(pop)
#endif
