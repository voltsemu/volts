#include "render.h"
#include "backend.h"

namespace volts::rsx
{
    struct vk : render
    {
        virtual ~vulkan() override {}

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

    namespace vulkan
    {
        void connect()
        {
            add(new vk());
        }
    }
}