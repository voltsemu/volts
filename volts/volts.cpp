#include "volts.h"

#include "svl/platform.h"

#if SYS_WINDOWS
#   include <windows.h>
#endif

#ifndef VTESTING

int main(int argc, char** argv)
{
#if SYS_WINDOWS
    // UTF-8 console output for windows
    SetConsoleOutputCP(CP_UTF8);
    setvbuf(stdout, nullptr, _IOFBF, 1024);
#endif
    
    // parse command line arguments
    volts::cmd::get().parse(argc, argv);
}

#else

#include "svl/tests.h"

int main(int argc, char** argv)
{
#if SYS_WINDOWS
    // UTF-8 console output for windows
    SetConsoleOutputCP(CP_UTF8);
    setvbuf(stdout, nullptr, _IOFBF, 1024);
#endif
    svl::tests::run_tests();
}

#endif