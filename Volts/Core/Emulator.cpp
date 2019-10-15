#include "Emulator.h"

#include <iostream>
#include <filesystem>

#include "imgui.h"
#include "imgui/examples/imgui_impl_glfw.h"

namespace Volts
{
    using namespace std;
    using namespace rapidjson;
    Emulator* Emulator::Get()
    {
        static Emulator* Singleton = new Emulator();
        return Singleton;
    }

    void Emulator::Run()
    {
        Config.Parse(R"(
            {
                "name": "jeff"
            }
        )");

        Frame.Open();

        ImGui_ImplGlfw_InitForOpenGL(Frame, true);
        Render.Current()->Attach();

        while(!glfwWindowShouldClose(Frame))
        {
            auto* RenderBackend = Render.Current();
            RenderBackend->Begin();
            ImGui_ImplGlfw_NewFrame();

            ImGui::NewFrame();
            ImGui::Begin("AAA");
            ImGui::End();
            ImGui::Render();

            RenderBackend->End();
            glfwPollEvents();
        }

        Render.Current()->Detach();
        ImGui_ImplGlfw_Shutdown();

        Frame.Close();
    }
}