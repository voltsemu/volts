#include "OpenGL.h"

namespace Volts::Render
{
    static OpenGL* Singleton = new OpenGL();
    OpenGL::OpenGL()
    {
        Emulator::Get()->Render.Register(this);
    }
}