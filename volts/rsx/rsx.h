#pragma once

#include <string>
#include <string_view>

namespace volts::rsx
{
    // represents a graphics rendering device
    struct device
    {
        virtual ~device() {}

        // gets the name of the device
        virtual const std::wstring& name() const = 0;
    };

    // represents a rendering backend
    struct render
    {
        virtual ~render() {}

        // attach this backend to a window
        virtual void attach(struct GLFWwindow* window) = 0;

        // detach the backend from the current window
        virtual void detach() = 0;

        // begin rendering a frame
        virtual void begin() = 0;

        // finish rendering a frame
        virtual void end() = 0;

        // name of the backend
        virtual std::string_view name() const = 0;
    };
}