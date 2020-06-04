#include <svl/platform.h>

#if SYS_WINDOWS

int WINAPI wWinMain(
    HINSTANCE inst,
    HINSTANCE prev,
    PWSTR cmd,
    int show
)
{
    (void)inst;
    (void)prev;
    (void)cmd;
    (void)show;
    return 0;
}

#else

int main(int argc, const char **argv)
{
    (void)argc;
    (void)argv;
    return 0;
}

#endif