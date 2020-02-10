#pragma once

#include <vector>
#include <string>

namespace volts::rsx
{
    // rendering device
    struct device
    {   
        virtual ~device() {}
        virtual const std::wstring& name() const = 0;
    };

    struct render
    {
        virtual ~render() {}
        virtual void attach() = 0;
        virtual void detach() = 0;

        virtual void begin() = 0;
        virtual void end() = 0;

        virtual std::string name() const = 0;

        virtual rsx::device* devices(int* len) const = 0;
    };

    std::vector<render*>& renders();
    void add(render* r);
}