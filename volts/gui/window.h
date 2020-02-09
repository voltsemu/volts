#pragma once

#include <string>

namespace volts::gui
{
    void init();
    void deinit();

    void set_title(const std::string& str);
}