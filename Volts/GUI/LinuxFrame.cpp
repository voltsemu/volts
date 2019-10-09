#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>

namespace Volts::GUI
{
    Size Frame::GetSize() const
    {

    }

    // TODO test all this
    void Frame::Run()
    {
        FinalizeRenders();
        PreInit();

        Display* Dis = XOpenDisplay(nullptr);
        int S = DefaultScreen(Dis);
        Window W  XCreateSimpleWindow(Dis, RootPanel(Dis, S), 10, 10, 100, 100, 1, BlackPixel(Dis, S), WhitePixel(Dis, S));
        XSelectInput(Dis, W, ExposureMask | KeyPressMask);
        XMapWindow(Dis, W);

        XEvent Evt;
        for(;;)
        {
            XNextEvent(Dis, &Evt);
            if(Evt.type == Expose)
                XFillRectangle(Dis, W, DefaultGC(Dis, S), 20, 20, 10, 10);
            CurrentRender->BeginRender();

            ImGui::NewFrame();
            GUILoop();
            ImGui::Render();

            CurrentRender->PresentRender();
        }

        XCloseDisplay(Dis);
    }
}