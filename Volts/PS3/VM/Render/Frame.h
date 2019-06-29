#pragma once

// basically the only header we can have here because objective C++
#include <Meta/Macros.h>

#include "Render.h"

// if we're on a platform other than apple we can include
// whatever we want, so we might as well take advantage of it
#if OS_WINDOWS
#   include <Windows.h>
#elif OS_LINUX
#   include <X11/Xlib.h>
#   include <X11/Xos.h>
#   include <X11/Xutil.h>
#endif

// this is going to have alot of #if in it
// trust me, its easier than the alternatives
// Justification:
//  - Q: Why not use Qt?
//  - A: this awnser comes in many parts
//       1. moc'ing is slow and involves an external build system that would be more hassle than its worth
//       2. Qt in general is too big for something of this scale.
//          this is an emulator, not a bulletproof bank interface.
//       3. Qt would grow the binary size by alot. upwards of 30MB. as of June 10, 2019 the binary size is 326KB
//       4. It would be yet another external depenency. this would slow down the build significantly
//          this would also lead to build system mixing and matching. making building very painful.
//  - Q: ok then, if not Qt why not another crossplatform GUI?
//  - A: i've looked at many, juce meets my needs but is paid
//       GuiLite exists but doesnt look nice.
//       wxWidgets is just ugly
//       GTK3 is even more ugly
//       Electron is just *NO*
//       the rest dont support all 3 target platforms
//  - Q: why not use XXX windowing library?
//  - A: see all my critisisms of Qt
//       also add performance into that because chances are XXX runs badly :^)
//  - Q: "and also why the fuck aren't you using something like winit instead of using every windowing system individually"
//  - A: see all of the above criticisms of Qt and apply them to that answer

namespace Volts::PS3::RSX
{

#if OS_WINDOWS
    using FrameHandle = HWND;
    // on windows every window has a callback function for input
    using WindowCallback = decltype(DefWindowProc);

    // we also need these on windows to open windows
    extern int CmdShow;
    extern HINSTANCE Instance;
#elif OS_LINUX
    using FrameHandle = Window;
#elif OS_APPLE
    using FrameHandle = void*; // NSView*
#endif

    // actually represents the entire window but hush
    struct Frame
    {
        // we have different versions of this class based on which os we're building for
        // this is done so 1. i dont tear my hair out trying to make a sane interface
        // and 2. it'll make it faster and easier to interface with stuff like vulkan, dx and metal
#if OS_WINDOWS
        void InputHandle(WindowCallback* Callback);
#endif

#if OS_WINDOWS
#       define WINDOWS_THIS this
#else
#       define WINDOWS_THIS
#endif

#if OS_WINDOWS
        void Create(Render* Parent);
#else
        void Create();
#endif

        void Show();
        void Close();
        FrameHandle GetHandle() const;

#if OS_WINDOWS
        // you better only call this once before calling PollEvents() or bad things are going to happen
        bool ShouldStayOpen();
        void PollEvents();
#endif

        Cthulhu::F32 AspectRatio() const { return ((Cthulhu::F32)Width) / ((Cthulhu::F32)Height); }

        unsigned Width, Height;
        unsigned X, Y;
        const char* Title;
    private:
        FrameHandle Handle;
#if OS_LINUX
        Display* D;
#endif

#if OS_WINDOWS
        // the last message we got from the event loop
        MSG LastMessage;
        // the callback we give to the OS to poll events with
        WindowCallback* InputCallback;
#endif
    };
}