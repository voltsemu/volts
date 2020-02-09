#include "svl/platform.h"

#if SYS_WINDOWS
#   include <windows.h>
#endif

#include <GLFW/glfw3.h>

#include "window.h"

#include <spdlog/spdlog.h>

namespace volts::gui
{
    static GLFWwindow* window = nullptr;

    void init()
    {
        if(!glfwInit())
        {
            // error
        }

        glfwSetErrorCallback([](int err, const char* desc) {
            spdlog::error("glfw error: {} {}", err, desc);
        });

        // we will provide our own graphics api stuff
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window = glfwCreateWindow(640, 480, "Volts", nullptr, nullptr);

        if(!window)
        {
            // error
        }

        while(!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
        }
    }

    void deinit()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void set_title(const std::string& str)
    {
        glfwSetWindowTitle(window, str.c_str());
    }
}