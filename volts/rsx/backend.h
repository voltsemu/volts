#pragma once

#include <string>

#include <types.h>
#include <expected.h>
#include <result.h>

struct GLFWwindow;

namespace volts::rsx
{
    struct render
    {
        virtual ~render() {}
        // called before window creation
        virtual void preinit(const std::string& name) = 0;

        // called after window creation
        virtual void postinit() = 0;

        // start rendering a frame
        virtual void begin() = 0;

        // display the rendered frame
        virtual void end() = 0;

        // finish rendering and cleanup
        virtual void cleanup() = 0;

        virtual std::string_view name() const = 0;
    };

    GLFWwindow* window();

    void add(render* r);
}