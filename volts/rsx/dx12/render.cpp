#include "render.h"
#include "backend.h"

namespace volts::rsx
{
    struct dx12 : render
    {
        virtual ~dx12() override {}

        virtual void preinit() override
        {

        }

        virtual void postinit() override
        {

        }

        virtual void begin() override
        {

        }

        virtual void end() override
        {

        }

        virtual void cleanup() override
        {

        }

        virtual const char* name() const override { return "d3d12"; }
    };

    void directx12::connect()
    {
        rsx::add(new dx12());
    }
}