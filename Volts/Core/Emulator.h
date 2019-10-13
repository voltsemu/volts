#pragma once

#include "Meta/Macros.h"

#include "Components/Audio/Audio.h"
#include "Components/Input/Input.h"
#include "Components/Files/FileSystem.h"
#include "Components/Render/Render.h"

#include "Frame/Window.h"

#include <vector>

#include "rapidjson/document.h"

namespace Volts
{
    template<typename T>
    struct Backends
    {
        void Register(T* Backend);
        T* Current() const { return Backends[Index]; }
        U32 Index = 0;
        const char** Names = nullptr;
    private:
        std::vector<T*> Backends;
    };

    struct Emulator
    {
        // get global singleton
        static Emulator* Get();

        void Run();

        rapidjson::Document Config;

        Window Frame;

        Backends<Audio::Audio> Audio;
        Backends<Input::Input> Input;
        Backends<Render::Render> Render;
    };
}