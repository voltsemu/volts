#pragma once

namespace volts::rsx
{
    struct vk
    {
        void init(const char* name, const char* version) { (void)name; (void)version; }
        void deinit() {}

        void begin() {}
        void end() {}
    };
}