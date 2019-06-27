#include "Render.h"

namespace Volts::PS3::RSX
{
    OpenGL::OpenGL()
    {
        Render::Register(this);
    }

    InitError OpenGL::Init(RenderDevice*)
    {
        return InitError::Ok;
    }

    void OpenGL::DeInit()
    {

    }

    static OpenGL* OGLSingleton = new OpenGL();
}