#include "volts.h"

#include "svl/platform.h"

#if SYS_WINDOWS
#   include <cstdio>
#   include <windows.h>
#endif

#ifdef VTESTING
#   include "svl/tests.h"
#endif

#ifdef VGUI
#   include "volts/gui/window.h"
#endif

#if defined(VGUI) && SYS_WINDOWS
// windows guis require a sepcific entry point
int WINAPI wWinMain(
    HINSTANCE inst,
    HINSTANCE prev,
    PWSTR cmd,
    int show
)
#else
// everything else is standard c++
int main(int argc, char** argv)
#endif
{
#if SYS_WINDOWS && !defined(VGUI)
    // UTF-8 console output for windows
    SetConsoleOutputCP(CP_UTF8);
    setvbuf(stdout, nullptr, _IOFBF, 1024);
#endif

#if defined(VTESTING)
    // testing build
    svl::tests::run_tests();
#elif defined(VGUI)
    volts::gui::init();

    volts::gui::deinit();
#else
    // command line only build
    volts::cmd::parse(argc, argv);
#endif
}
