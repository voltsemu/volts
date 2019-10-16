#pragma once

#include "Meta/Macros.h"


#define GLFW_INCLUE_VULKAN
#include <GLFW/glfw3.h>

#if OS_APPLE
#   define GLFW_EXPOSE_NATIVE_COCOA
#elif OS_WIN
#   define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include <GLFW/glfw3native.h>

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