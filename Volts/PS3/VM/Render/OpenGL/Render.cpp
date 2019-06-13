#include "Render.h"

namespace Volts::PS3::RSX
{
    OpenGL::OpenGL()
    {
        MessageBox(nullptr, "OpenGL Ctor", "Volts", 0);
    }

    InitError OpenGL::Init()
    {
        return InitError::Ok;
    }

    void OpenGL::DeInit()
    {

    }

    static OpenGL* OGLSingleton = new OpenGL();
}