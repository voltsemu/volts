#pragma once

#include "Meta/Macros.h"


#define GLFW_INCLUE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

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