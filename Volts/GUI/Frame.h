#include <Core/Collections/CthulhuString.h>

#if OS_WINDOWS
#   include <Windows.h>
#   include <d3d12.h>
#   include <dxgi1_4.h>
#   include <tchar.h>
#   include <wrl.h>
#elif OS_APPLE
// TODO: what includes
#elif OS_LINUX
// TODO: window includes
#endif

namespace Volts::GUI
{
    using namespace Cthulhu;

#if OS_WINDOWS
    extern HINSTANCE Instance;
#endif

    struct Frame
    {
        Frame(U32 W, U32 H, U32 X, U32 Y, const String& Title);
#if OS_WINDOWS
        using FrameHandle = HWND;
#elif OS_APPLE
        using FrameHandle = void*; // NSWindow
#elif OS_LINUX
        using FrameHandle = void*; // IDK
#endif
        FrameHandle Handle;
    };
}