#pragma once

#include <string>

#include <external/imgui/imgui.h>

namespace volts::gui
{
    void init();
    void deinit();

    struct GLFWwindow* handle();

    void set_title(const std::string& str);

    struct ImGuiLog
    {
        ImGuiTextBuffer buf;
        ImGuiTextFilter filter;
        bool need_scroll;
    };

    extern ImGuiLog log;
}