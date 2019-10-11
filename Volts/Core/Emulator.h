#pragma once

#include <Core/Collections/Array.h>

#include "Logger.h"

#include "GUI/Frame.h"
#include "Render/Render.h"
#include "Audio/Player.h"
#include "Input/Controller.h"

namespace Volts
{
    using namespace Cthulhu;

    struct Emulator
    {
        static Emulator& Get();
        void Run();

        GUI::Frame Window = {};

        void GUI();

        ImGuiTextBuffer LogBuffer;
        Level CurrentLevel = Level::Info;
        void Log(Level L, std::string&& Message);

        // render backends
        void Register(RSX::Render* Backend);
        Array<RSX::Render*> RenderBackends = {};
        const char** RenderNames = nullptr;
        U32 RenderIndex = 0;
        RSX::Render& CurrentRender() { return *RenderBackends[RenderIndex]; }


        // audio backends
        void Register(Audio::Player* Backend);
        Array<Audio::Player*> AudioBackends = {};
        const char** AudioNames = nullptr;
        U32 AudioIndex = 0;
        Audio::Player& CurrentPlayer() { return *AudioBackends[AudioIndex]; }


        // input device backends
        void Register(Input::Controller* Backend);
        Array<Input::Controller*> InputBackends = {};
        const char** InputNames = nullptr;
        U32 InputIndex = 0;
        Input::Controller& CurrentController() { return *InputBackends[InputIndex]; }
    };
}