#include "rsx.h"

#include "dxutil.h"

#include "d3d12x.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "gui/window.h"

#include "imgui/examples/imgui_impl_dx12.h"
#include "imgui/examples/imgui_impl_glfw.h"

#define NUM_FRAMES 2
#define MIN_FEATURE_LEVEL D3D_FEATURE_LEVEL_11_0

namespace volts::rsx
{
    struct per_frame
    {
        ComPtr<ID3D12Resource> target;
        ComPtr<ID3D12CommandAllocator> allocator;
        UINT64 fence_val;
    };

    struct dx12 : render
    {
        dx12() { }

        virtual void attach() override
        {
            HWND window = glfwGetWin32Window(gui::handle());

            ComPtr<IDXGIFactory4> factory;
            ENSURE(L"creating factory failed", CreateDXGIFactory2(0, IID_PPV_ARGS(&factory)));

            ENSURE(L"creating device failed", D3D12CreateDevice(nullptr, MIN_FEATURE_LEVEL, IID_PPV_ARGS(&device)));
        }

        virtual void detach() override
        {

        }

        virtual std::string name() const override { return "DirectX12"; }


        // pipeline data

        CD3DX12_VIEWPORT port;
        CD3DX12_RECT scissor;

        ComPtr<IDXGISwapChain3> swap;
        ComPtr<ID3D12Device> device;
        ComPtr<ID3D12CommandQueue> queue;
        ComPtr<ID3D12RootSignature> root;
        ComPtr<ID3D12DescriptorHeap> heap;
        ComPtr<ID3D12PipelineState> state;
        ComPtr<ID3D12GraphicsCommandList> commands;
        UINT heap_size;

        // sync data
        UINT frame_idx;
        HANDLE fence_event;
        ComPtr<ID3D12Fence> fence;


        // data per swap frame
        per_frame frame_data[NUM_FRAMES];
    };

    static dx12* singleton = new dx12();
}