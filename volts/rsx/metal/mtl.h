#pragma once

namespace volts::rsx
{
    struct mtl
    {
        void init(const char* name, const char* version) { (void)name; (void)version; }
        void deinit() {}

        void begin() {}
        void end() {}
    };
}
