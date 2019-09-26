#pragma once

#include "Render/Render.h"

#if OS_WINDOWS
#   include <GL/glew.h>
#elif OS_LINUX
#   include <GL/glew.h>
#elif OS_APPLE
#   include <OpenGL/gl.h>
#endif

namespace Volts::RSX
{
    struct OpenGL : Render
    {
        OpenGL();

    private:
    };
}