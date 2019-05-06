#include <Volts.h>

namespace Volts::PS3
{
    struct PS3 : public Emulator
    {
        virtual ~PS3() override {}

        virtual void Run() override
        {
            printf("Yeet\n");
        }

        virtual Cthulhu::String Name() override
        {
            return "PS3";
        }
    };

    REGISTER_EMULATOR(PS3);
}