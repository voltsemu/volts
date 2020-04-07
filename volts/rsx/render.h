#pragma once

#include <string>
#include <map>

namespace volts::rsx
{
    // run the rsx window using a backend
    void run(const std::string& render_name, const char* game_name, const char* version);
}