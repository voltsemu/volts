#include "Render.h"

namespace Volts::RSX
{
    //static OpenGL* Singleton = new OpenGL();
    OpenGL::OpenGL()
    {
        GUI::Frame::GetSingleton()->AddRender(this);
    }
}