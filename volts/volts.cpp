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
    setvbuf(stdout, nullptr, _IOFBF, 1024);
#endif
    
    // parse command line arguments
    cmd::get().parse(argc, argv);
}
