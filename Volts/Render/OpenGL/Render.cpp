#include "Render.h"

namespace Volts::RSX
{
    static OpenGL* Singleton = new OpenGL();
    OpenGL::OpenGL()
    {
        Emulator::Get().Register(this);
    }

    OpenGL::~OpenGL()
    {

    }

    void OpenGL::Attach(GUI::Frame* Handle)
    {
        // TODO: figure out how to get glew working at all
        Frame = Handle;
    }

    void OpenGL::Detach()
    {

    }

    void OpenGL::UpdateVSync(bool Enabled)
    {

    }

    Device* OpenGL::Devices(U32* Count)
    {
        *Count = 0;
        return nullptr;
    }

    void OpenGL::Windowed() {}
    void OpenGL::Fullscreen() {}
    void OpenGL::Borderless() {}

    void OpenGL::Resize(GUI::Size NewSize)
    {

    }

    void OpenGL::BeginRender()
    {

    }

    void OpenGL::PresentRender()
    {

    }
}