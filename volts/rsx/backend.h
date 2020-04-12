#pragma once

#include <string>

#include <svl/types.h>
#include <svl/expected.h>
#include <svl/result.h>

struct GLFWwindow;

namespace volts::rsx
{
    struct game
    {
        const char* title;
        const char* version;
    };

    struct render
    {
        virtual ~render() {}
        // called before window creation
        virtual void preinit(const game& game) = 0;

        // called after window creation
        virtual void postinit(GLFWwindow* window) = 0;

        // start rendering a frame
        virtual void begin() = 0;

        // display the rendered frame
        virtual void end() = 0;

        // finish rendering and cleanup
        virtual void cleanup() = 0;

        virtual const char* name() const = 0;
    };

    void add(render* r);
}