#pragma once

#include <Meta/Macros.h>
#include <Meta/Aliases.h>

#include "Macros.h"

#include "Components/Audio/Audio.h"
#include "Components/Input/Input.h"
#include "Components/Files/FileSystem.h"
#include "Components/Render/Render.h"

#include "Frame/Window.h"

#include "Config.h"

#if OS_WIN
#   include <windows.h>
#endif

#define FMT_EXCEPTIONS 0
#include <fmt/format.h>
#include <vector>
#include <filesystem>
#include "imgui/imgui.h"

#define VTRACE(...) Volts::Trace(fmt::format(__VA_ARGS__).c_str());
#define VINFO(...) Volts::Info(fmt::format(__VA_ARGS__).c_str());
#define VWARN(...) Volts::Warn(fmt::format(__VA_ARGS__).c_str());
#define VERROR(...) Volts::Error(fmt::format(__VA_ARGS__).c_str());
#define VFATAL(...) Volts::Fatal(fmt::format(__VA_ARGS__).c_str());

namespace Volts
{
    using namespace Cthulhu;

    void Trace(const char*);
    void Info(const char*);
    void Warn(const char*);
    void Error(const char*);
    void Fatal(const char*);

    template<typename T>
    struct Backends
    {
        Backends(const char* M) : RegisterMessage(M) {}

        template<typename TOther>
        void Register(TOther* Backend)
        {
            static_assert(std::is_base_of<T, TOther>::value);
            BackendList.push_back((T*)Backend);
            VINFO(RegisterMessage, Backend->Name());
        }

        T* Current() const { return BackendList[Index]; }

        void Finalize()
        {
            Count = BackendList.size();
            Names = new const char*[Count];
            for(U32 I = 0; I < Count; I++)
            {
                Names[I] = BackendList[I]->Name();
            }
        }

        void Update(U32 NewIndex)
        {
            if(NewIndex != Index)
            {
                Current()->Detach();
                Index = NewIndex;
                Current()->Attach();
            }
        }

        I32 Index = 0;
        I32 Count = 0;
        const char** Names = nullptr;
    private:
        std::vector<T*> BackendList;
        const char* RegisterMessage;
    };

    enum class LogLevel : U8
    {
        Trace = 0,
        Info = 1,
        Warn = 2,
        Error = 3,
        Fatal = 4,
    };

    struct Emulator
    {
        // get global singleton
        static Emulator* Get();

        void Run();

        Window Frame;

        Backends<Audio::Audio> Audio{"Registered {} audio backend"};
        Backends<Input::Input> Input{"Registered {} input backend"};
        Backends<Render::Render> Render{"Registered {} render backend"};

#if OS_WIN
        HINSTANCE Instance;
#endif

        ImGuiTextBuffer LogBuffer;
        LogLevel Level = LogLevel::Info;

        Config Cfg = {};
    private:
        void GUI();
    };
}