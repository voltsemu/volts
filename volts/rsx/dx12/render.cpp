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

            tearing = can_tear();

            UINT flags = 0;

            spdlog::debug("validation enabled");
            ComPtr<ID3D12Debug> debug;
            DX_ENSURE(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)));
            debug->EnableDebugLayer();
            flags |= DXGI_CREATE_FACTORY_DEBUG;

            DX_ENSURE(CreateDXGIFactory2(flags, IID_PPV_ARGS(&factory)));

            IDXGIAdapter1* adapt;

            for(int i = 0; factory->EnumAdapters1(i, &adapt) != DXGI_ERROR_NOT_FOUND; i++)
                adapters.push_back(adapt);
        }

        virtual void postinit(GLFWwindow* win) override
        {
            window = win;

            int w, h;
            glfwGetFramebufferSize(win, &w, &h);

            viewport = CD3DX12_VIEWPORT(
                0.f, 0.f,
                static_cast<float>(w), static_cast<float>(h)
            );

            scissor = CD3DX12_RECT(0, 0, w, h);

            create_device();
            load_device();
            create_backbuffer();
            load_data();

            ImGui_ImplGlfw_InitForOpenGL(win, true);

            ImGui_ImplDX12_Init(
                device.Get(),
                DX_FRAME_COUNT,
                DXGI_FORMAT_R8G8B8A8_UNORM,
                srvheap.Get(),
                srvheap->GetCPUDescriptorHandleForHeapStart(),
                srvheap->GetGPUDescriptorHandleForHeapStart()
            );
        }

        virtual void begin() override
        {
            ImGui_ImplDX12_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }

        virtual void end() override
        {
            ImGui::Render();
            populate_commands();
            ID3D12CommandList* command_lists[] = { commands[frame].Get() };
            queue->ExecuteCommandLists(_countof(command_lists), command_lists);

            UINT flags = tearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

            DX_ENSURE(swap->Present(0, flags));
            advance();
        }

        virtual void cleanup() override
        {
            ImGui_ImplDX12_Shutdown();
            ImGui_ImplGlfw_Shutdown();
        }

        virtual const char* name() const override { return "d3d12"; }

    private:
        GLFWwindow* window;

        void create_backbuffer();
        void create_device();
        void load_device();
        void load_data();

        void populate_commands();
        void wait();
        void advance();

        ComPtr<IDXGIFactory4> factory;
        std::vector<IDXGIAdapter1*> adapters;
        int device_index = 0;

        IDXGIAdapter1* get_device() const { return adapters[device_index]; }

        CD3DX12_VIEWPORT viewport;
        CD3DX12_RECT scissor;

        ComPtr<IDXGISwapChain3> swap;
        ComPtr<ID3D12Device> device;
        ComPtr<ID3D12CommandQueue> queue;

        ComPtr<ID3D12Resource> targets[DX_FRAME_COUNT];
        ComPtr<ID3D12CommandAllocator> allocators[DX_FRAME_COUNT];

        ComPtr<ID3D12RootSignature> signature;
        ComPtr<ID3D12PipelineState> state;
        ComPtr<ID3D12GraphicsCommandList> commands[DX_FRAME_COUNT];
        ComPtr<ID3D12DescriptorHeap> rtvheap;
        UINT rtvsize;

        ComPtr<ID3D12DescriptorHeap> srvheap;

        ComPtr<ID3D12Resource> buffer;
        D3D12_VERTEX_BUFFER_VIEW buffer_view;

        UINT frame;
        HANDLE fence_event;
        ComPtr<ID3D12Fence> fence;
        UINT64 fence_values[DX_FRAME_COUNT];

        bool tearing = false;

#if DX_VALIDATE
        ComPtr<ID3D12InfoQueue> debug_queue;
#endif
    };

    void d3d12::create_backbuffer()
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        
        desc.NumDescriptors = DX_FRAME_COUNT;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        DX_ENSURE(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&rtvheap)));

        desc.NumDescriptors = DX_FRAME_COUNT;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        DX_ENSURE(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&srvheap)));

        CD3DX12_CPU_DESCRIPTOR_HANDLE handle(rtvheap->GetCPUDescriptorHandleForHeapStart());

        for(int i = 0; i < DX_FRAME_COUNT; i++)
        {
            DX_ENSURE(swap->GetBuffer(i, IID_PPV_ARGS(&targets[i])));
            device->CreateRenderTargetView(targets[i].Get(), nullptr, handle);

            handle.Offset(1, rtvsize);

            DX_ENSURE(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocators[i])));
        }
    }

    void d3d12::create_device()
    {
        DX_ENSURE(D3D12CreateDevice(
            get_device(),
            D3D_FEATURE_LEVEL_12_0,
            IID_PPV_ARGS(&device)
        ));

        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        DX_ENSURE(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&queue)));
    }

    void d3d12::load_device()
    {
        int w, h;
        glfwGetWindowSize(window, &w, &h);

        DXGI_SWAP_CHAIN_DESC1 desc = {};
        desc.BufferCount = DX_FRAME_COUNT;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        desc.Flags = tearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
        desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
        desc.Width = w;
        desc.Height = h;
        desc.SampleDesc.Count = 1;

        ComPtr<IDXGISwapChain1> swapchain;
        DX_ENSURE(factory->CreateSwapChainForHwnd(
            queue.Get(),
            glfwGetWin32Window(window),
            &desc,
            nullptr,
            nullptr,
            &swapchain
        ));

        DX_ENSURE(factory->MakeWindowAssociation(
            glfwGetWin32Window(window),
            DXGI_MWA_NO_ALT_ENTER
        ));

        DX_ENSURE(swapchain.As(&swap));

        frame = swap->GetCurrentBackBufferIndex();
    }

    void d3d12::load_data()
    {
        D3D12_ROOT_SIGNATURE_DESC desc = {};
        desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        ComPtr<ID3DBlob> root;
        ComPtr<ID3DBlob> error;
        DX_ENSURE(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &root, &error));
        DX_ENSURE(device->CreateRootSignature(
            0,
            root->GetBufferPointer(),
            root->GetBufferSize(),
            IID_PPV_ARGS(&signature)
        ));

        ComPtr<ID3DBlob> vert;
        ComPtr<ID3DBlob> pixel;

#if DX_VALIDATE
        UINT flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        UINT flags = 0;
#endif

        const char* source = R"(
            struct psinput
            {
                float4 position : SV_POSITION;
                float4 colour : COLOR;
            };

            psinput vsmain(float4 pos : POSITION, float4 colour : COLOUR)
            {
                psinput output;
                output.position = pos;
                output.colour = colour;
                return output;
            }

            float4 psmain(psinput input) : SV_TARGET
            {
                return input.colour;
            }
        )";

        DX_ENSURE(D3DCompile(
            source,
            strlen(source),
            "vertex",
            nullptr,
            nullptr,
            "vsmain",
            "vs_5_0",
            flags,
            0,
            &vert,
            &error
        ));

        //spdlog::error("err {}", (char*)error->GetBufferPointer());

        DX_ENSURE(D3DCompile(
            source,
            strlen(source),
            "pixel",
            nullptr,
            nullptr,
            "psmain",
            "ps_5_0",
            flags,
            0,
            &pixel,
            &error
        ));

        D3D12_INPUT_ELEMENT_DESC elements[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOUR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        D3D12_GRAPHICS_PIPELINE_STATE_DESC pso = {};
        pso.InputLayout = { elements, _countof(elements) };
        pso.pRootSignature = signature.Get();
        pso.VS = CD3DX12_SHADER_BYTECODE(vert.Get());
        pso.PS = CD3DX12_SHADER_BYTECODE(pixel.Get());

        D3D12_RASTERIZER_DESC rd = {};
        rd.FillMode = D3D12_FILL_MODE_SOLID;
        rd.CullMode = D3D12_CULL_MODE_BACK;
        rd.FrontCounterClockwise = false;
        rd.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
        rd.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
        rd.DepthClipEnable = true;
        rd.MultisampleEnable = false;
        rd.AntialiasedLineEnable = false;
        rd.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

        pso.RasterizerState = rd;

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

        pso.BlendState = bd;

        pso.DepthStencilState.DepthEnable = false;
        pso.DepthStencilState.StencilEnable = false;
        pso.SampleMask = UINT_MAX;
        pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        pso.NumRenderTargets = 1;
        pso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        pso.SampleDesc.Count = 1;

        DX_ENSURE(device->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&state)));

        for(int i = 0; i < DX_FRAME_COUNT; i++)
        {
            DX_ENSURE(device->CreateCommandList(
                0,
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                allocators[i].Get(),
                state.Get(),
                IID_PPV_ARGS(&commands[i])
            ));

            DX_ENSURE(commands[i]->Close());
        }

        vertex verts[] = {
            { { 0.f, 0.25f, 0.f }, { 1.f, 0.f, 0.f, 1.f } },
            { { 0.25f, -0.25f, 0.f }, { 0.f, 1.f, 0.f, 1.f } },
            { { -0.25f, -0.25f, 0.f }, { 0.f, 0.f, 1.f, 1.f } }
        };

        DX_ENSURE(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(sizeof(verts)),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&buffer)
        ));

        byte* data;
        CD3DX12_RANGE range(0, 0);
        DX_ENSURE(buffer->Map(0, &range, reinterpret_cast<void**>(&data)));
        std::memcpy(data, verts, sizeof(verts));
        buffer->Unmap(0, nullptr);

        buffer_view.BufferLocation = buffer->GetGPUVirtualAddress();
        buffer_view.StrideInBytes = sizeof(vertex);
        buffer_view.SizeInBytes = sizeof(verts);

        DX_ENSURE(device->CreateFence(
            fence_values[frame],
            D3D12_FENCE_FLAG_NONE,
            IID_PPV_ARGS(&fence)
        ));

        fence_values[frame]++;

        fence_event = CreateEvent(nullptr, false, false, nullptr);
        if(!fence_event)
        {
            DX_ENSURE(GetLastError());
        }

        wait();
    }

    void d3d12::populate_commands()
    {
        auto cmd = commands[frame].Get();
        DX_ENSURE(allocators[frame]->Reset());
        DX_ENSURE(cmd->Reset(allocators[frame].Get(), state.Get()));

        cmd->SetGraphicsRootSignature(signature.Get());
        cmd->RSSetViewports(1, &viewport);
        cmd->RSSetScissorRects(1, &scissor);

        cmd->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
            targets[frame].Get(),
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET
        ));

        D3D12_CPU_DESCRIPTOR_HANDLE handle = { rtvheap->GetCPUDescriptorHandleForHeapStart().ptr + (frame * rtvsize) };
        cmd->OMSetRenderTargets(1, &handle, false, nullptr);

        ID3D12DescriptorHeap* srv[] = { srvheap.Get() };
        cmd->SetDescriptorHeaps(1, srv);

        const float clear[] = { 0.f, 0.2f, 0.4f, 1.f };

        cmd->ClearRenderTargetView(handle, clear, 0, nullptr);
        cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd->IASetVertexBuffers(0, 1, &buffer_view);
        cmd->DrawInstanced(3, 1, 0, 0);

        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmd);

        cmd->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
            targets[frame].Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT
        ));

        DX_ENSURE(cmd->Close());
    }

    void d3d12::wait()
    {
        DX_ENSURE(queue->Signal(fence.Get(), fence_values[frame]));
        DX_ENSURE(fence->SetEventOnCompletion(fence_values[frame], fence_event));
        WaitForSingleObjectEx(fence_event, INFINITE, false);

        fence_values[frame]++;
    }

    void d3d12::advance()
    {
        auto current = fence_values[frame];
        DX_ENSURE(queue->Signal(fence.Get(), current));

        frame = swap->GetCurrentBackBufferIndex();

        if(fence->GetCompletedValue() < fence_values[frame])
        {
            DX_ENSURE(fence->SetEventOnCompletion(fence_values[frame], fence_event));
            WaitForSingleObjectEx(fence_event, INFINITE, false);
        }

        fence_values[frame] = current + 1;
    }

    void directx12::connect()
    {
        rsx::add(new d3d12());
    }
}