#pragma once

#include <Meta/Macros.h>
#include <Meta/Aliases.h>
#include <Core/Collections/CthulhuString.h>

namespace Volts::PS3::RSX
{

#if OS_WINDOWS
    using FrameHandle = HWND;
#elif OS_LINUX
    using FrameHandle = Window;
#elif OS_APPLE
    using FrameHandle = void*; // NSView*
#endif

    struct Frame
    {
        void SetHeight(Cthulhu::U32 H);
        void SetWidth(Cthulhu::U32 W);
        void SetTitle(Cthulhu::String& T);
        void Create();
        FrameHandle GetHandle() const;

    private:
        Cthulhu::U32 Width, Height;
        Cthulhu::String Title;
        FrameHandle Handle;
    };
}