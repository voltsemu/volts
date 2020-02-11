#include <platform.h>

#include "gui/window.h"

#if SYS_WINDOWS
#   include <cstdio>
#   include <windows.h>
#endif

#if SYS_WINDOWS
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
    volts::window::run();
}
