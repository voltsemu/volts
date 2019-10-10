#pragma once

#include <Core/Collections/Array.h>

#include "GUI/Frame.h"
#include "Render/Render.h"
#include "Audio/Player.h"
#include "Input/Controller.h"

namespace Volts
{
    using namespace Cthulhu;

    struct Emulator
    {
        void Run();

        GUI::Frame Window;

        void Register(RSX::Render* Backend);
        Array<RSX::Render*> RenderBackends = {};

        void Register(Audio::Player* Backend);
        Array<Audio::Player*> AudioBackends = {};

        void Register(Input::Controller* Backend);
        Array<Input::Controller*> InputBackends = {};
    };

    extern Emulator Emu;
}