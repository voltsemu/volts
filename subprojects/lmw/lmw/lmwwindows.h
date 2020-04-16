#pragma once

#include <stdint.h>
#include <Windows.h>
#include <tuple>

namespace lmw
{
    struct window
    {
        window() = delete;
        window(const window&) = delete;
        window(window&&) = delete;
        window(const char* title, uint32_t width, uint32_t height);

        std::tuple<uint32_t, uint32_t> size() const;

        void close();

        bool poll();

        void rename(const char* title);

        HWND handle() const;

    private:
        HWND win;
    };

    const char** vk_extensions(uint32_t* num);
}