#include "render.h"
#include "backend.h"

namespace volts::rsx
{
    struct vk : render
    {
        virtual ~vk() override {}

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
    };

    void vulkan::connect()
    {
        add(new vk());
    }
}