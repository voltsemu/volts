#include "render.h"
#include "backend.h"

#include <spdlog/spdlog.h>

#include "imgui/imgui.h"

#include "imgui/examples/imgui_impl_opengl3.h"
#include "imgui/examples/imgui_impl_glfw.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace volts::rsx
{
    struct ogl : render
    {
        virtual ~ogl() override {}

        virtual void preinit(const game& game) override
        {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        }

        virtual void postinit(GLFWwindow* window) override
        {
            win = window;
            // enable vsync (TODO)
            glfwMakeContextCurrent(window);
            //glfwSwapInterval(1);

            gladLoadGL();

            glfwGetFramebufferSize(window, &width, &height);

            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init("#version 430");
        }

        virtual void begin() override
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }

        virtual void end() override
        {
            glViewport(0, 0, width, height);
            glClearColor(0.45f, 0.55f, 0.60f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(win);
        }

        virtual void cleanup() override
        {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }

        virtual const char* name() const override { return "opengl"; }

    private:
        GLFWwindow* win;
        int width;
        int height;
    };

    void opengl::connect()
    {
        rsx::add(new ogl());
    }
}