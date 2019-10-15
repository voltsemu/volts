#pragma once

#include "Meta/Macros.h"

#include <GLFW/glfw3.h>

namespace Volts
{
    struct Window
    {
        Window();
        ~Window();

        void Open();
        void Close();

        Size GetSize() const;

    private:
        GLFWwindow* Handle;
    };
}