#include "svl/platform.h"

#if SYS_WINDOWS
#   include <windows.h>
#endif

#include <GLFW/glfw3.h>

#include "window.h"

#include <spdlog/spdlog.h>

#include <external/alert/alert.h>

#include <spdlog/sinks/base_sink.h>

namespace volts::gui
{
    static GLFWwindow* window = nullptr;

    ImGuiLog log;

/*
    ImGuiTextBuffer log_buf;

    template<typename Mutex>
    struct imgui_sink : spdlog::sinks::base_sink<Mutex>
    {
        void sink_it_(const spdlog::details::log_msg& msg) override
        {
            spdlog::memory_buf_t formatted;
            base_sink<Mutex>::formatter_->format(msg, formatted);
            std::cout << fmt::to_string(formatted);
        }

        void flush_() override 
        {

        }
    };
*/

    void init()
    {
        glfwSetErrorCallback([](int err, const char* desc) {
            spdlog::error("glfw error: {} {}", err, desc);
        });

        if(!glfwInit())
        {
            alert::alert(L"GLFW error", L"Failed to initialize glfw");
        }

        // we will provide our own graphics api stuff
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window = glfwCreateWindow(640, 480, "Volts", nullptr, nullptr);

        if(!window)
        {
            alert::alert(L"GLFW error", L"Failed to initialize glfw window");
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

    GLFWwindow* handle() { return window; }
}