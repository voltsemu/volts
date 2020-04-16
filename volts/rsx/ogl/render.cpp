#include "render.h"

#include <lmw/lmw.h>

#include <spdlog/spdlog.h>

#include "imgui/imgui.h"

#include "imgui/examples/imgui_impl_opengl3.h"
#include "imgui/examples/imgui_impl_glfw.h"

#include <glad/glad.h>
//#include <GLFW/glfw3.h>

namespace volts::rsx
{
    struct ogl : render
    {
        virtual ~ogl() override {}

        virtual void preinit(const game& game) override
        {
            gladLoadGL();
        }

        virtual void postinit(lmw::window* window) override
        {
            win = window;

            init_gl();

            std::tie(width, height) = window->size();
            // enable vsync (TODO)
            //glfwMakeContextCurrent(window);
            //glfwSwapInterval(1);

            //glfwGetFramebufferSize(window, &width, &height);

            //ImGui_ImplGlfw_InitForOpenGL(window, true);
            //ImGui_ImplOpenGL3_Init("#version 430");
        }

        virtual void begin() override
        {
            //ImGui_ImplOpenGL3_NewFrame();
            //ImGui_ImplGlfw_NewFrame();
            //ImGui::NewFrame();
        }

        virtual void end() override
        {
            glViewport(0, 0, width, height);
            glClearColor(0.45f, 0.55f, 0.60f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT);

            //ImGui::Render();
            //ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            //glfwSwapBuffers(win);
        }

        virtual void cleanup() override
        {
            //ImGui_ImplOpenGL3_Shutdown();
            //ImGui_ImplGlfw_Shutdown();
            //ImGui::DestroyContext();
            deinit_gl();
        }

        virtual const char* name() const override { return "opengl"; }

    private:
        void init_gl()
        {
#if SYS_WINDOWS
        hdc = GetDC(win->handle());
        PIXELFORMATDESCRIPTOR pfd = {};
        pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;

        auto pf = ChoosePixelFormat(hdc, &pfd);

        SetPixelFormat(hdc, pf, &pfd);

        DescribePixelFormat(hdc, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

        hrc = wglCreateContext(hdc);
        wglMakeCurrent(hdc, hrc);
#else

#endif
        }

        void deinit_gl()
        {
#if SYS_WINDOWS
            wglMakeCurrent(nullptr, nullptr);
            ReleaseDC(win->handle(), hdc);
            wglDeleteContext(hrc);
#else

#endif
        }

#if SYS_WINDOWS
        HDC hdc;
        HGLRC hrc;
#else

#endif
        lmw::window* win;
        int width;
        int height;
    };

    render* opengl::connect()
    {
        return new ogl();
    }
}