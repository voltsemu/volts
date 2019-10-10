#include "Emulator.h"

namespace Volts
{
    Emulator Emu = {};

    void Emulator::Run()
    {
        //Window = GUI::Frame()
        //    .SetTitle("Volts")
        //    .Run();
    }

    void Emulator::Register(RSX::Render* Backend)
    {
        VINFO("Registered {} rendering backend", Backend->Name());
        RenderBackends.Append(Backend);
    }

    void Emulator::Register(Audio::Player* Backend)
    {
        VINFO("Registered {} audio backend", Backend->Name());
        AudioBackends.Append(Backend);
    }

    void Emulator::Register(Input::Controller* Backend)
    {
        VINFO("Registered {} input backend", Backend->Name());
        InputBackends.Append(Backend);
    }
}