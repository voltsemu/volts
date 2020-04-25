#pragma once

namespace volts::rsx
{
    struct gl
    {
        void init(const char* name, const char* version) { (void)name; (void)version; }
        void deinit() {}

        void begin() {}
        void end() {}
    };
}
