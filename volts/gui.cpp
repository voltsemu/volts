#include <platform.h>

void vmain();

#if SYS_WINDOWS

int wWinMain(HINSTANCE inst, HINSTANCE prev, PWSTR cmd, int show)
{
    vmain();
    return 0;
}

#else

#include <stdio.h>

int main(int argc, const char** argv)
{
    vmain();
    return 0;
}

#endif

void vmain()
{

}