#include "rsx.h"

#include "dxutil.h"

#include "d3d12x.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "gui/window.h"

#include "imgui/examples/imgui_impl_dx12.h"

#define NUM_FRAMES 2
#define MIN_FEATURE_LEVEL D3D_FEATURE_LEVEL_11_0

namespace volts::rsx
{
    struct dx12 : render
    {
        dx12() 
        {
            rsx::add(this);
        }

        // attach to the main window
        virtual void attach() override
        {
            int width, height;
            glfwGetWindowSize(gui::handle(), &width, &height);

            viewport = {
                0.f, 0.f,
                (float)width, (float)height
            };

            scissor = {
                0, 0,
                width, height
            };

            load_device_data();
            load_data();

            ImGui_ImplDX12_Init(
                device.Get(),
                NUM_FRAMES,
                DXGI_FORMAT_R8G8B8A8_UNORM,
                heap.Get(),
                heap->GetCPUDescriptorHandleForHeapStart(),
                heap->GetGPUDescriptorHandleForHeapStart()
            );
            
            ComPtr<IDXGIFactory4> factory;
            ENSURE(L"creating factory failed", CreateDXGIFactory2(0, IID_PPV_ARGS(&factory)));

            // get first available gpu
            ComPtr<IDXGIAdapter1> adapter;
            dxutil::get_adapter(factory.Get(), &adapter);

            // create device from first gpu
            ENSURE(L"creating device failed", D3D12CreateDevice(adapter.Get(), MIN_FEATURE_LEVEL, IID_PPV_ARGS(&device)));

            {
                D3D12_COMMAND_QUEUE_DESC qd = {};
                qd.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
                qd.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

                ENSURE(L"failed to create command queue", device->CreateCommandQueue(&qd, IID_PPV_ARGS(&queue)));
            }

            {
                int width, height;
                glfwGetWindowSize(gui::handle(), &width, &height);

                HWND window = glfwGetWin32Window(gui::handle());

                DXGI_SWAP_CHAIN_DESC1 scd = {};
                scd.BufferCount = NUM_FRAMES;
                scd.Width = width;
                scd.Height = height;
                scd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
                scd.SampleDesc.Count = 1;

                ComPtr<IDXGISwapChain1> chain;
                ENSURE(L"failed to create swapchain", factory->CreateSwapChainForHwnd(
                    queue.Get(),
                    window,
                    &scd,
                    nullptr,
                    nullptr,
                    &chain
                ));

                ENSURE(L"failed to cast swapchain", chain.As(&swap));
                frame_idx = swap->GetCurrentBackBufferIndex();
            }

            {
                D3D12_DESCRIPTOR_HEAP_DESC dhd = {};
                dhd.NumDescriptors = NUM_FRAMES;
                dhd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
                dhd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
                ENSURE(L"failed to create descriptor heap", device->CreateDescriptorHeap(&dhd, IID_PPV_ARGS(&heap)));

                heap_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            }

            {
                CD3DX12_CPU_DESCRIPTOR_HANDLE handle(heap->GetCPUDescriptorHandleForHeapStart());

                for(UINT i = 0; i < NUM_FRAMES; i++)
                {
                    ENSURE(L"failed to get swapchain buffer", swap->GetBuffer(i, IID_PPV_ARGS(&frame_data[i].target)));
                    device->CreateRenderTargetView(frame_data[i].target.Get(), nullptr, handle);
                    handle.Offset(1, heap_size);

                    ENSURE(L"failed to create command allocator", device->CreateCommandAllocator(
                        D3D12_COMMAND_LIST_TYPE_DIRECT,
                        IID_PPV_ARGS(&frame_data[i].allocator)
                    ));
                }
            }

            {
                CD3DX12_ROOT_SIGNATURE_DESC rsd = {};
                rsd.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

                ComPtr<ID3DBlob> sig;
                ComPtr<ID3DBlob> err;

                ENSURE(L"failed to serialize root signature", D3D12SerializeRootSignature(&rsd, D3D_ROOT_SIGNATURE_VERSION_1, &sig, &err));
                ENSURE(L"failed to create root signature", device->CreateRootSignature(
                    0,
                    sig->GetBufferPointer(),
                    sig->GetBufferSize(),
                    IID_PPV_ARGS(&root)
                ));
            }

            {
                ComPtr<ID3DBlob> vert;
                ComPtr<ID3DBlob> pixel;

                const char* shader = R"(
                    struct ps_input
                    {
                        float4 pos : SV_POS;
                        float4 col : COLOUR;
                    };

                    ps_input vs_main(float4 pos : POS, float4 col : COLOUR)
                    {
                        ps_input res;
                        
                        res.pos = pos;
                        res.col = col;

                        return res;
                    }

                    float4 ps_main(ps_input in) : SV_TARGET
                    {
                        return in.col;
                    }
                )";
                size_t len = strlen(shader);

                ENSURE(L"failed to compile vertex shader", D3DCompile(
                    shader,
                    len,
                    nullptr,
                    nullptr,
                    D3D_COMPILE_STANDARD_FILE_INCLUDE,
                    "vs_main",
                    "vs_5_0",
                    0,
                    0,
                    &vert,
                    nullptr
                ));

                ENSURE(L"failed to compile pixel shader", D3DCompile(
                    shader,
                    len,
                    nullptr,
                    nullptr,
                    D3D_COMPILE_STANDARD_FILE_INCLUDE,
                    "ps_main",
                    "ps_5_0",
                    0,
                    0,
                    &pixel,
                    nullptr
                ));

                D3D12_INPUT_ELEMENT_DESC ieds[] = {
                    { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                    { "COLOUR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }  
                };

                D3D12_GRAPHICS_PIPELINE_STATE_DESC psd = {};
                psd.InputLayout = { ieds, _countof(ieds) };
                psd.pRootSignature = root.Get();
                psd.VS = CD3DX12_SHADER_BYTECODE(vert.Get());
                psd.PS = CD3DX12_SHADER_BYTECODE(pixel.Get());
                psd.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
                psd.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
                psd.DepthStencilState.DepthEnable = false;
                psd.DepthStencilState.StencilEnable = false;
                psd.SampleMask = UINT_MAX;
                psd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
                psd.NumRenderTargets = 1;
                psd.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
                psd.SampleDesc.Count = 1;
                ENSURE(L"failed to create pipeline state", device->CreateGraphicsPipelineState(
                    &psd,
                    IID_PPV_ARGS(&state)
                ));
            }

            ENSURE(L"failed to create command list", device->CreateCommandList(
                0,
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                frame_data[frame_idx].allocator.Get(),
                state.Get(),
                IID_PPV_ARGS(&commands)
            ));

            ENSURE(L"failed to close command list", commands->Close());

            // todo: setup drawing and stuff

            {
                ENSURE(L"failed to create fence", device->CreateFence(
                    frame_data[frame_idx].fence_val,
                    D3D12_FENCE_FLAG_NONE,
                    IID_PPV_ARGS(&fence)
                ));

                frame_data[frame_idx].fence_val++;

                fence_event = CreateEvent(nullptr, false, false, nullptr);
                if(fence_event == nullptr)
                {
                    // print error TODO
                }
            }

            wait_for_gpu();
        }

        virtual void detach() override
        {
            wait_for_gpu();
            CloseHandle(fence_event);
            ImGui_ImplDX12_Shutdown();
            // TODO: proper cleanup
        }

        virtual void begin() override
        {
            ImGui_ImplDX12_NewFrame();
        }

        virtual void end() override
        {

        }

        virtual std::string name() const override { return "DirectX12"; }

        // internal functions

        void load_device_data()
        {

        }

        void load_data()
        {

        }

        void wait_for_gpu()
        {
            ENSURE(L"failed to signal queue", queue->Signal(fence.Get(), frame_data[frame_idx].fence_val));

            ENSURE(L"failed to set event", fence->SetEventOnCompletion(frame_data[frame_idx].fence_val, fence_event));
            WaitForSingleObjectEx(fence_event, INFINITE, false);

            frame_data[frame_idx].fence_val++;
        }

        // pipeline data

        CD3DX12_VIEWPORT viewport;
        CD3DX12_RECT scissor;

        ComPtr<IDXGISwapChain3> swap;
        ComPtr<ID3D12Device> device;
        ComPtr<ID3D12Resource> targets[NUM_FRAMES];
        ComPtr<ID3D12CommandQueue> queue;
        ComPtr<ID3D12RootSignature> root;
        ComPtr<ID3D12DescriptorHeap> heap;
        ComPtr<ID3D12PipelineState> state;
        ComPtr<ID3D12GraphicsCommandList> commands;
        ComPtr<ID3D12CommandAllocator> allocators[NUM_FRAMES];
        UINT heap_size;

        // sync data
        UINT frame_idx;
        HANDLE fence_event;
        ComPtr<ID3D12Fence> fence;
        UINT64 fence_vals[NUM_FRAMES];
    };

    static dx12* singleton = new dx12();
}