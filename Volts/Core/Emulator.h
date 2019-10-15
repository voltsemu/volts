#pragma once

#include "Meta/Macros.h"

#include "Components/Audio/Audio.h"
#include "Components/Input/Input.h"
#include "Components/Files/FileSystem.h"
#include "Components/Render/Render.h"

#include "Frame/Window.h"

#include <vector>
#include <filesystem>

#include "rapidjson/document.h"

#define VINFO(...)
#define VWARN(...)
#define VERROR(...)
#define VFATAL(...)

namespace Volts
{
    template<typename T>
    struct Backends
    {
        Backends(const char* M) : RegisterMessage(M) {}
        void Register(T* Backend)
        {
            BackendList.push_back(Backend);
            VINFO(RegisterMessage, Backend->Name());
        }

        T* Current() const { return BackendList[Index]; }
        U32 Index = 0;
        const char** Names = nullptr;
    private:
        std::vector<T*> BackendList;
        const char* RegisterMessage;
    };

    struct Emulator
    {
        // get global singleton
        static Emulator* Get();

        void Run();

        rapidjson::Document Config;

        Window Frame;

        Backends<Audio::Audio> Audio{"Registered {} audio backend"};
        Backends<Input::Input> Input{"Registered {} input backend"};
        Backends<Render::Render> Render{"Registered {} render backend"};
    };
}