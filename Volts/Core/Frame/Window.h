#pragma once

#include "Meta/Macros.h"

#include <GLFW/glfw3.h>

#if OS_APPLE
#   define GLFW_EXPOSE_NATIVE_COCOA
#endif

#import <GLFW/glfw3native.h>

namespace Volts
{
    struct Window
    {
        Window();
        ~Window();

        void Open();
        void Close();

        operator GLFWwindow*() const { return Handle; }
    private:
        GLFWwindow* Handle;
    };
}