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
        virtual void preinit() = 0;

        // called after window creation
        virtual void postinit() = 0;

        // start rendering a frame
        virtual void begin() = 0;

        // display the rendered frame
        virtual void end() = 0;

        // finish rendering and cleanup
        virtual void cleanup() = 0;

        virtual const char* name() const = 0;
    };

    GLFWwindow* window();
    const char* title();
    uint32_t version();

    void add(render* r);
}