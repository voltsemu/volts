#pragma once

#include <vector>
#include <string>

namespace volts::rsx
{
    struct render
    {
        virtual ~render() {}
        virtual void attach() = 0;
        virtual void detach() = 0;

        virtual std::string name() const = 0;
    };

    std::vector<render*>& renders();
    void add(render* r);
}