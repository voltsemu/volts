#include "Emulator.h"

#include <iostream>
#include <filesystem>
#include <chrono>

#include "glfw/glfw3.h"

#include "imgui.h"
#include "imgui/examples/imgui_impl_glfw.h"
#include "imgui/examples/imgui_impl_win32.h"
#include "imfilebrowser.h"

#include "Utils/SFO.h"
#include "Utils/UNSELF.h"

namespace Volts
{
    using namespace Utils;

    void Trace(const char* M)
    {
        auto* Emu = Emulator::Get();
        if(Emu->Level > LogLevel::Trace)
            return;

        *Emu->OutStream << fmt::format("[trace] {}\n", M);
    }

    void Info(const char* M)
    {
        auto* Emu = Emulator::Get();
        if(Emu->Level > LogLevel::Info)
            return;

        *Emu->OutStream << fmt::format("[info] {}\n", M);
    }

    void Warn(const char* M)
    {
        auto* Emu = Emulator::Get();
        if(Emu->Level > LogLevel::Warn)
            return;

        *Emu->OutStream << fmt::format("[info] {}\n", M);
    }

    void Error(const char* M)
    {
        auto* Emu = Emulator::Get();
        if(Emu->Level > LogLevel::Error)
            return;

        *Emu->OutStream << fmt::format("[error] {}\n", M);
    }

    void Fatal(const char* M)
    {
        *Emulator::Get()->OutStream << fmt::format("[fatal] {}", M);
    }

    using namespace std;

    Emulator* Emulator::Get()
    {
        static Emulator* Singleton = new Emulator();
        return Singleton;
    }

    Emulator::Emulator()
    {
        OutStream = &std::cout;
    }

    void Emulator::Run()
    {
        //auto& IO = ImGui::GetIO();
        //IO.IniFilename = "Config/imgui.ini";

        Render.Finalize();
        Input.Finalize();
        Audio.Finalize();

        // glfwSetErrorCallback([](int Error, const char* Desc) {
        //     VERROR("GLFW error: {}:{}", Error, Desc);
        // });

        // if(!glfwInit())
        // {
        //     VFATAL("Failed to initialize glfw");
        //     return;
        // }

        // auto* Window = glfwCreateWindow(780, 480, "Volts", nullptr, nullptr);
        // if(!Window)
        // {
        //     VFATAL("Failed to create glfw window");
        //     return;
        // }

        // glfwDestroyWindow(Window);

        // glfwTerminate();
    }
}