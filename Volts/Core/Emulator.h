#pragma once

#include "PCH.h"

#include <Cthulhu/System/Binary.h>

#include "Macros.h"

#include "Components/Audio/Audio.h"
#include "Components/Input/Input.h"
#include "Components/Render/Render.h"

#if OS_WIN
#   include <windows.h>
#endif

#include "Generated/Version.h"

#define VTRACE(...) { Volts::Trace(fmt::format(__VA_ARGS__).c_str()); }
#define VINFO(...) { Volts::Info(fmt::format(__VA_ARGS__).c_str()); }
#define VWARN(...) { Volts::Warn(fmt::format(__VA_ARGS__).c_str()); }
#define VERROR(...) { Volts::Error(fmt::format(__VA_ARGS__).c_str()); }
#define VFATAL(...) { Volts::Fatal(fmt::format(__VA_ARGS__).c_str()); }

namespace Volts
{
    using namespace Cthulhu;
    using namespace Types;

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

        std::vector<T*> BackendList;
        const char* RegisterMessage;
    };

    enum class LogLevel : Types::U8
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

        Emulator();

        void Run();

        Backends<Audio::Audio> Audio{"Registered {} audio backend"};

        void UpdateDeviceNames();

        Backends<Input::Input> Input{"Registered {} input backend"};
        Backends<Render::Render> Render{"Registered {} render backend"};

        LogLevel Level = LogLevel::Info;

        std::ostream* OutStream;
    };
}