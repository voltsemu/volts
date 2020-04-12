#include "render.h"
#include "backend.h"
#include "support.h"

#define GLFW_EXPOSE_NATIVE_WIN32

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_dx12.h"
#include "imgui/examples/imgui_impl_glfw.h"

#define DX_FRAME_COUNT 2

namespace volts::rsx
{
    struct vertex
    {
        glm::vec3 position;
        glm::vec4 colour;
    };

    struct d3d12 : render
    {
        virtual ~d3d12() override {}

        virtual void preinit(const game& game) override
        {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        }

        virtual void postinit(GLFWwindow* win) override
        {
            window = win;
        }

        virtual void begin() override
        {

        }

        virtual void end() override
        {

        }

        virtual void cleanup() override
        {

        }

        virtual const char* name() const override { return "d3d12"; }

    private:
        GLFWwindow* window;

        ComPtr<ID3D12Device> device;
    };

    void directx12::connect()
    {
        rsx::add(new d3d12());
    }
}