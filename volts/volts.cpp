#include "volts.h"

#include "svl/platform.h"

using namespace volts;

#if SYS_WINDOWS
#   include <windows.h>
#endif

int main(int argc, char** argv)
{

#if SYS_WINDOWS
    // UTF-8 console output for windows
    SetConsoleOutputCP(CP_UTF8);
    setvbuf(stdout, nullptr, _IOFBF, 1000);
#endif

    for(int i = 0; i < argc; i++)
        spdlog::info(argv[i]);
    
    cmd::get().parse(argc, argv);

    system("pause");
}

#if SYS_WINDOWS

// todo: once this app needs to create a window
// we will need to implement wWinMain
// but for now we dont so this can stay empty

#endif