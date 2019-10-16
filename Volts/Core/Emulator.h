#pragma once

#include "Meta/Macros.h"

#include "Components/Audio/Audio.h"
#include "Components/Input/Input.h"
#include "Components/Files/FileSystem.h"
#include "Components/Render/Render.h"

#include "Frame/Window.h"

#if OS_WIN
#   include <windows.h>
#endif

#define FMT_EXCEPTIONS 0
#include <fmt/format.h>
#include <vector>
#include <filesystem>

#include "rapidjson/document.h"

#define VINFO(Fmt, ...) fmt::print("[info]: {}\n", fmt::format(Fmt, __VA_ARGS__));
#define VWARN(Fmt, ...) fmt::print("[warn]: {}\n", fmt::format(Fmt, __VA_ARGS__));
#define VERROR(Fmt, ...) fmt::print("[error]: {}\n", fmt::format(Fmt, __VA_ARGS__));
#define VFATAL(Fmt, ...) fmt::print("[fatal]: {}\n", fmt::format(Fmt, __VA_ARGS__));

namespace Volts
{
    template<typename T>
    struct Backends
    {
        Backends(const char* M) : RegisterMessage(M) {}

        template<typename TOther>
        void Register(TOther* Backend)
        {
            BackendList.push_back((T*)Backend);
            VINFO(RegisterMessage, Backend->Name());
        }

        T* Current() const { return BackendList[Index]; }
        void Set(const char* N)
        {
            for(U32 I = 0; I < BackendList.size(); I++)
            {
                if(strcmp(BackendList[I]->Name(), N) == 0)
                {
                    Index = I;
                    break;
                }
            }
        }
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

#if OS_WIN
        HINSTANCE Instance;
#endif
    };
}