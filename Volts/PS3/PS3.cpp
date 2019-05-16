#include <Volts.h>

namespace Volts::PS3
{
    struct Emulator : public Volts::Emulator
    {
        virtual ~Emulator() override {}

        virtual void Init() override {}
        virtual void Run() override {}
        virtual void Pause() override {}
        virtual Cthulhu::Byte* SaveState() const override { return nullptr; }
        virtual void Quit() override {}
        virtual Cthulhu::String Name() const override { return "PS3"; }
        virtual Cthulhu::String Description() const override { return "Sony PlayStation 3"; }
    };

    REGISTER_EMULATOR(Volts::PS3::Emulator);
}