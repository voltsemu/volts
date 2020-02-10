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
            // add itself to the render index
            rsx::add(this);

            // create our factory
            VALIDATE(CreateDXGIFactory2(0, IID_PPV_ARGS(&factory)));

            tear = dxutil::can_tear(factory.Get());

            // get a list of all devices setup
            IDXGIAdapter1* adapter;

            for(UINT i = 0; factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; i++)
                device_list.push_back(dxutil::dx12device(adapter));
        }

        ~dx12() override {}

        /// external interface
        
        // attach to the main window
        virtual void attach() override
        {
            // get the size of the viewports
            int width, height;
            glfwGetWindowSize(gui::handle(), &width, &height);

            viewport = CD3DX12_VIEWPORT(0.f, 0.f, (float)width, (float)height);

            scissor = CD3DX12_RECT(0, 0, width, height);

            load_device_data();
            load_data();

            // connect imgui to our dx12 context
            ImGui_ImplDX12_Init(
                device.Get(),
                NUM_FRAMES,
                DXGI_FORMAT_R8G8B8A8_UNORM,
                srvheap.Get(),
                srvheap->GetCPUDescriptorHandleForHeapStart(),
                srvheap->GetGPUDescriptorHandleForHeapStart()
            );
        }

        virtual void detach() override
        {
            ImGui_ImplDX12_Shutdown();
        }

        virtual void begin() override
        {
            ImGui_ImplDX12_NewFrame();
        }

        virtual void end() override
        {
            populate_command_list();
            ID3D12CommandList* list[] = { commands.Get() };
            queue->ExecuteCommandLists(_countof(list), list);

            // todo: vsync and stuff
            UINT flags = tear ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

            VALIDATE(swap->Present(0, flags));
            advance_frame();
        }

        virtual std::string name() const override { return "DirectX12"; }

        virtual rsx::device* devices(int* len) const override 
        { 
            *len = device_list.size();
            return (rsx::device*)device_list.data();
        }

        // internal functions

        void create_device_data() 
        {
            VALIDATE(D3D12CreateDevice(
                current_device(),
                MIN_FEATURE_LEVEL,
                IID_PPV_ARGS(&device)
            ));

            {
                D3D12_COMMAND_QUEUE_DESC cqd = {};
                cqd.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
                cqd.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
                VALIDATE(device->CreateCommandQueue(
                    &cqd,
                    IID_PPV_ARGS(&queue)
                ));
            }
        }

        void load_device_data() 
        {
            create_device_data();

            {
                DXGI_SWAP_CHAIN_DESC1 scd = {};
                scd.BufferCount = NUM_FRAMES;

                int w, h;

                glfwGetWindowSize(gui::handle(), &w, &h);

                scd.Width = w;
                scd.Height = h;

                scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
                scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
                scd.SampleDesc.Count = 1;
                scd.Flags = tear ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

                ComPtr<IDXGISwapChain1> chain;
                VALIDATE(factory->CreateSwapChainForHwnd(
                    queue.Get(),
                    glfwGetWin32Window(gui::handle()),
                    &scd,
                    nullptr,
                    nullptr,
                    &chain
                ));

                VALIDATE(factory->MakeWindowAssociation(
                    glfwGetWin32Window(gui::handle()),
                    DXGI_MWA_NO_ALT_ENTER
                ));

                VALIDATE(chain.As(&swap));
            }

            frame_idx = swap->GetCurrentBackBufferIndex();

            create_back_buffer();
        }

        void load_sized_resources() 
        {
            int width, height;
            glfwGetWindowSize(gui::handle(), &width, &height);

            viewport = CD3DX12_VIEWPORT(0.f, 0.f, (float)width, (float)height);

            scissor = CD3DX12_RECT(0, 0, width, height);

            {
                CD3DX12_CPU_DESCRIPTOR_HANDLE rtvhandle(rtvheap->GetCPUDescriptorHandleForHeapStart());

                for(int i = 0; i < NUM_FRAMES; i++)
                {
                    VALIDATE(swap->GetBuffer(i, IID_PPV_ARGS(&targets[i])));
                    device->CreateRenderTargetView(targets[i].Get(), nullptr, rtvhandle);
                    rtvhandle.Offset(1, desc_size);

                    VALIDATE(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocators[i])));
                }
            }
        }

        void load_data() 
        {
            {
                D3D12_ROOT_SIGNATURE_DESC rsd = {};
                rsd.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

                ComPtr<ID3DBlob> sig;
                ComPtr<ID3DBlob> err;
                VALIDATE(D3D12SerializeRootSignature(&rsd, D3D_ROOT_SIGNATURE_VERSION_1, &sig, &err));
                VALIDATE(device->CreateRootSignature(
                    0,
                    sig->GetBufferPointer(),
                    sig->GetBufferSize(),
                    IID_PPV_ARGS(&root)
                ));
            }

            {
                std::string src = R"(
                    struct ps_input
                    {
                        float4 position : SV_POSITION;
                        float4 colour : COLOUR;
                    };

                    ps_input vs_main(float4 position : POSITION, float4 colour : COLOUR)
                    {
                        ps_input out;

                        out.position = position;
                        out.colour = colour;

                        return out;
                    }

                    float4 ps_main(ps_input in) : SV_TARGET
                    {
                        return in.colour;
                    }
                )";

                ComPtr<ID3DBlob> vert;
                ComPtr<ID3DBlob> pixel;

                VALIDATE(D3DCompile(
                    src.c_str(),
                    src.size(),
                    "source",
                    nullptr,
                    nullptr,
                    "vs_main",
                    "vs_5_0",
                    0,
                    0,
                    &vert,
                    nullptr
                ));

                VALIDATE(D3DCompile(
                    src.c_str(),
                    src.size(),
                    "sources",
                    nullptr,
                    nullptr,
                    "ps_main",
                    "ps_5_0",
                    0,
                    0,
                    &pixel,
                    nullptr
                ));

                D3D12_INPUT_ELEMENT_DESC ied[] = {
                    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                    { "COLOUR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
                };

                D3D12_GRAPHICS_PIPELINE_STATE_DESC psd = {};
                psd.InputLayout = { ied, _countof(ied) };
                psd.pRootSignature = root.Get();
                psd.VS = CD3DX12_SHADER_BYTECODE(vert.Get());
                psd.PS = CD3DX12_SHADER_BYTECODE(pixel.Get());

                D3D12_RASTERIZER_DESC rd = {};
                rd.FillMode = D3D12_FILL_MODE_SOLID;
                rd.CullMode = D3D12_CULL_MODE_BACK;
                rd.FrontCounterClockwise = false;
                rd.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
                rd.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
                rd.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
                rd.DepthClipEnable = true;
                rd.MultisampleEnable = false;
                rd.AntialiasedLineEnable = false;
                rd.ForcedSampleCount = 0;
                rd.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

                psd.RasterizerState = rd;

                D3D12_BLEND_DESC bd = {};
                bd.AlphaToCoverageEnable = false;
                bd.IndependentBlendEnable = false;

                D3D12_RENDER_TARGET_BLEND_DESC rtbd = {
                    false, false,
                    D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
                    D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
                    D3D12_LOGIC_OP_NOOP,
                    D3D12_COLOR_WRITE_ENABLE_ALL
                };

                for(int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
                    bd.RenderTarget[i] = rtbd;

                psd.BlendState = bd;

                psd.DepthStencilState.DepthEnable = false;
                psd.DepthStencilState.StencilEnable = false;
                psd.SampleMask = UINT_MAX;
                psd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
                psd.NumRenderTargets = 1;
                psd.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
                psd.SampleDesc.Count = 1;
                VALIDATE(device->CreateGraphicsPipelineState(&psd, IID_PPV_ARGS(&state)));

                VALIDATE(device->CreateCommandList(
                    0,
                    D3D12_COMMAND_LIST_TYPE_DIRECT,
                    allocators[frame_idx].Get(),
                    state.Get(),
                    IID_PPV_ARGS(&commands)
                ));

                VALIDATE(commands->Close());
            }

            {
                VALIDATE(device->CreateFence(
                    fence_vals[frame_idx],
                    D3D12_FENCE_FLAG_NONE,
                    IID_PPV_ARGS(&fence)
                ));
                fence_vals[frame_idx]++;

                fence_event = CreateEvent(nullptr, false, false, nullptr);

                if(!fence_event)
                {
                    // error TODO
                }

                wait_for_gpu();
            }
        }

        void wait_for_gpu() 
        {
            VALIDATE(queue->Signal(fence.Get(), fence_vals[frame_idx]));
            VALIDATE(fence->SetEventOnCompletion(fence_vals[frame_idx], fence_event));
            WaitForSingleObjectEx(fence_event, INFINITE, false);

            fence_vals[frame_idx]++;
        }

        void advance_frame() 
        {
            UINT64 current = fence_vals[frame_idx];
            VALIDATE(queue->Signal(fence.Get(), current));

            frame_idx = swap->GetCurrentBackBufferIndex();

            if(fence->GetCompletedValue() < fence_vals[frame_idx])
            {
                VALIDATE(fence->SetEventOnCompletion(fence_vals[frame_idx], fence_event));
                WaitForSingleObjectEx(fence_event, INFINITE, false);
            }

            fence_vals[frame_idx] = current + 1;
        }

        void create_back_buffer() 
        {
            {
                D3D12_DESCRIPTOR_HEAP_DESC dhd = {};
                dhd.NumDescriptors = NUM_FRAMES;
                dhd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
                dhd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
                VALIDATE(device->CreateDescriptorHeap(&dhd, IID_PPV_ARGS(&rtvheap)));

                desc_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            }

            {
                CD3DX12_CPU_DESCRIPTOR_HANDLE cpu_handle(rtvheap->GetCPUDescriptorHandleForHeapStart());

                for(int i = 0; i < NUM_FRAMES; i++)
                {
                    VALIDATE(swap->GetBuffer(i, IID_PPV_ARGS(&targets[i])));
                    device->CreateRenderTargetView(targets[i].Get(), nullptr, cpu_handle);
                    cpu_handle.Offset(1, desc_size);

                    VALIDATE(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocators[i])));
                }
            }

            {
                D3D12_DESCRIPTOR_HEAP_DESC dhd = {};
                dhd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
                dhd.NumDescriptors = 1;
                dhd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                VALIDATE(device->CreateDescriptorHeap(&dhd, IID_PPV_ARGS(&srvheap)));
            }
        }

        void populate_command_list() 
        {
            VALIDATE(allocators[frame_idx]->Reset());
            VALIDATE(commands->Reset(allocators[frame_idx].Get(), state.Get()));

            commands->SetGraphicsRootSignature(root.Get());
            commands->RSSetViewports(1, &viewport);
            commands->RSSetScissorRects(1, &scissor);

            commands->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
                targets[frame_idx].Get(),
                D3D12_RESOURCE_STATE_PRESENT,
                D3D12_RESOURCE_STATE_RENDER_TARGET
            ));

            D3D12_CPU_DESCRIPTOR_HANDLE rtvhandle = {};
            rtvhandle.ptr = rtvheap->GetCPUDescriptorHandleForHeapStart().ptr + (frame_idx * desc_size);
            commands->OMSetRenderTargets(1, &rtvhandle, false, nullptr);

            ID3D12DescriptorHeap* srv[] = { srvheap.Get() };

            commands->SetDescriptorHeaps(1, srv);

            // imgui stuff
            ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commands.Get());

            commands->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
                targets[frame_idx].Get(),
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_STATE_PRESENT
            ));

            VALIDATE(commands->Close());
        }

        // pipeline data

        CD3DX12_VIEWPORT viewport;
        CD3DX12_RECT scissor;

        ComPtr<IDXGISwapChain3> swap;
        ComPtr<ID3D12Device> device;
        ComPtr<ID3D12CommandQueue> queue;

        ComPtr<ID3D12Resource> targets[NUM_FRAMES];
        ComPtr<ID3D12CommandAllocator> allocators[NUM_FRAMES];

        ComPtr<ID3D12RootSignature> root;
        ComPtr<ID3D12PipelineState> state;
        ComPtr<ID3D12GraphicsCommandList> commands;
        ComPtr<ID3D12DescriptorHeap> rtvheap;
        UINT desc_size;

        ComPtr<ID3D12DescriptorHeap> srvheap;

        // sync data
        UINT frame_idx;
        HANDLE fence_event;
        ComPtr<ID3D12Fence> fence;
        UINT64 fence_vals[NUM_FRAMES];

        // other data
        std::vector<dxutil::dx12device> device_list;
        ComPtr<IDXGIFactory5> factory;
        bool tear = false;
        int device_idx = 0;
        IDXGIAdapter1* current_device() const { return device_list[device_idx].handle; }
    };

    static dx12* singleton = new dx12();
}