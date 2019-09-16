#include <Core/Collections/CthulhuString.h>

namespace Volts::GUI
{
    using namespace Cthulhu;

    struct Frame
    {
        Frame(U32 W, U32 H, const String& Title);
#if OS_WINDOWS
        using FrameHandle = HWND;
        using ViewHandle = void; // IDK
#elif OS_APPLE
        using FrameHandle = void*; // NSWindow
        using ViewHandle = void*; // MTKView
#elif OS_LINUX
        using FrameHandle = void*; // IDK
        using ViewHandle = void*; // IDK
#endif
        FrameHandle Handle;
        ViewHandle View;
    };
}