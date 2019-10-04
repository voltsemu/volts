#include "Render.h"
#include <Core/Memory/Memory.h>
#include "Frame.h"

#include "d3dx12.h"
#include "dxgidebug.h"

#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_win32.h"
#include "imgui/examples/imgui_impl_dx12.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

namespace Volts::RSX
{
    static DX12* Singleton = new DX12();

    DX12::DX12()
    {
        GUI::Frame::GetSingleton()->AddRender(this);

        UINT FactoryFlags = 0;

#if VDXDEBUG
        {
            Ptr<ID3D12Debug> Debugger;
            VALIDATE(D3D12GetDebugInterface(IID_PPV_ARGS(&Debugger)));
            Debugger->EnableDebugLayer();
            FactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
#endif

        VALIDATE(CreateDXGIFactory2(FactoryFlags, IID_PPV_ARGS(&Factory)));

        IDXGIAdapter1* Adapter;

        for(U32 I = 0; Factory->EnumAdapters1(I, &Adapter) != DXGI_ERROR_NOT_FOUND; I++)
            DeviceList.push_back(DX12Support::DX12Device(Adapter));
    }

    LRESULT CALLBACK DX12FrameProc(
        HWND Window,
        UINT Msg,
        WPARAM W,
        LPARAM L
    )
    {
        if(ImGui_ImplWin32_WndProcHandler(Window, Msg, W, L))
            return true;

        return DefWindowProc(Window, Msg, W, L);
    }

    void DX12::CreateChildWindow()
    {
        WNDCLASSEX WC = { sizeof(WNDCLASSEX) };
        WC.style = CS_HREDRAW | CS_VREDRAW;
        WC.lpfnWndProc = DX12FrameProc;
        WC.hInstance = GUI::Instance;
        WC.hCursor = LoadCursor(nullptr, IDC_ARROW);
        WC.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        WC.lpszClassName = "VoltsWindowSubClass";

        RegisterClassEx(&WC);

        RECT Size;
        GetWindowRect(Frame->Handle, &Size);

        Child = CreateWindowEx(
            0,
            "VoltsWindowSubClass",
            "Volts",
            WS_CHILD | WS_BORDER | WS_VISIBLE,
            Size.top, Size.left,
            Size.right, Size.bottom,
            Frame->Handle,
            nullptr,
            GUI::Instance,
            nullptr
        );

        Frame->SetChild(Child);
        ShowWindow(Child, SW_SHOW);
        SetForegroundWindow(Child);
        SetFocus(Child);
        UpdateWindow(Child);
    }

    void DX12::CleanupChildWindow()
    {
        CloseWindow(Child);
    }

    void DX12::Attach(GUI::Frame* Handle)
    {
        Frame = Handle;

        GUI::Size S = Handle->GetSize();
        Viewport = {
            0.0f,
            0.0f,
            static_cast<F32>(S.Width),
            static_cast<F32>(S.Height)
        };
        Scissor = {
            0,
            0,
            static_cast<LONG>(S.Width),
            static_cast<LONG>(S.Height)
        };
        LoadPipeline();
        LoadData();

        ImGui_ImplDX12_Init(
            Device.Get(),
            FrameCount,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            SRVHeap->GetCPUDescriptorHandleForHeapStart(),
            SRVHeap->GetGPUDescriptorHandleForHeapStart()
        );
    }

    void DX12::Detach()
    {
        WaitForGPU();
        CleanupChildWindow();
        ImGui_ImplDX12_Shutdown();

        CloseHandle(FenceEvent);
    }

    void DX12::Windowed()
    {

    }

    void DX12::Fullscreen()
    {

    }

    void DX12::Borderless()
    {

    }

    void DX12::Resize(GUI::Size NewSize)
    {

    }

    Device* DX12::Devices(U32* Count)
    {
        *Count = static_cast<U32>(DeviceList.size() - 1);
        return DeviceList.data();
    }

    void DX12::SetDevice(RSX::Device* Device)
    {
        for(U32 I = 0; I < DeviceList.size(); I++)
        {
            if(DeviceList[I].Name() == Device->Name())
            {
                CurrentDeviceIndex = I;

                WaitForGPU();
                ReleaseObjects();
                LoadPipeline();
                LoadData();

                break;
            }
        }
    }

    void DX12::Options()
    {
        const char* ShaderOptions[] = { "Level 0", "Level 1", "Level 2", "Level 3", "Debug" };
        static I32 CurrentLevel = 0;
        ImGui::Combo("Shader Compile Speed", &CurrentLevel, ShaderOptions, IM_ARRAYSIZE(ShaderOptions));
        ImGui::SameLine(); ImGui::HelpText("Set the optimization level of the shaders, the slower the option the faster the compiled shader.");

        switch(CurrentLevel)
        {
        case 0:
            ShaderOptimizationLevel = D3DCOMPILE_OPTIMIZATION_LEVEL0;
            break;
        case 1:
            ShaderOptimizationLevel = D3DCOMPILE_OPTIMIZATION_LEVEL1;
            break;
        case 2:
            ShaderOptimizationLevel = D3DCOMPILE_OPTIMIZATION_LEVEL2;
            break;
        case 3:
            ShaderOptimizationLevel = D3DCOMPILE_OPTIMIZATION_LEVEL3;
            break;
        case 4:
            ShaderOptimizationLevel = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
            break;
        default:
            break;
        }
    }

    void DX12::PopulateCommandList()
    {
        VALIDATE(CommandAllocators[FrameIndex]->Reset());
        VALIDATE(CommandList->Reset(CommandAllocators[FrameIndex].Get(), PipelineState.Get()));

        CommandList->SetGraphicsRootSignature(RootSignature.Get());
        CommandList->RSSetViewports(1, &Viewport);
        CommandList->RSSetScissorRects(1, &Scissor);

        CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
            RenderTargets[FrameIndex].Get(),
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET
        ));

        D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle = {};
        RTVHandle.ptr = RTVHeap->GetCPUDescriptorHandleForHeapStart().ptr + (FrameIndex * RTVDescriptorSize);
        CommandList->OMSetRenderTargets(1, &RTVHandle, false, nullptr);

        ID3D12DescriptorHeap* SRV[] = { SRVHeap.Get() };

        CommandList->SetDescriptorHeaps(1, SRV);

        const F32 ClearColour[] = { 0.0f, 0.2f, 0.4f, 1.0f };

        CommandList->ClearRenderTargetView(RTVHandle, ClearColour, 0, nullptr);
        CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        CommandList->IASetVertexBuffers(0, 1, &VertexBufferView);
        CommandList->DrawInstanced(3, 1, 0, 0);
        // render stuff in here

        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), CommandList.Get());

        CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
            RenderTargets[FrameIndex].Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT
        ));

        // stop rendering here

        VALIDATE(CommandList->Close());
    }

    void DX12::ReleaseObjects()
    {
        Fence.Reset();
        CommandQueue.Reset();
        Swap.Reset();
        Device.Reset();
        Factory.Reset();

#if VDXDEBUG
        {
            Ptr<IDXGIDebug1> Debug;
            if(SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&Debug))))
                Debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
        }
#endif
    }

    void DX12::BeginRender()
    {
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
    }

    void DX12::PresentRender()
    {
        PopulateCommandList();
        ID3D12CommandList* Commands[] = { CommandList.Get() };
        CommandQueue->ExecuteCommandLists(_countof(Commands), Commands);

        UINT PresentFlags = Tear ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

        VALIDATE(Swap->Present(VSyncMode, PresentFlags));
        AdvanceFrame();
    }

    void DX12::LoadPipeline()
    {
        VALIDATE(D3D12CreateDevice(
            DeviceList[CurrentDeviceIndex].Handle,
            D3D_FEATURE_LEVEL_12_0,
            IID_PPV_ARGS(&Device)
        ));

        {
            // create the command queue
            D3D12_COMMAND_QUEUE_DESC CQD = {};
            CQD.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
            CQD.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            VALIDATE(Device->CreateCommandQueue(
                &CQD,
                IID_PPV_ARGS(&CommandQueue)
            ));
        }

#if VDXDEBUG
        VALIDATE(Device->QueryInterface(IID_PPV_ARGS(&DebugQueue)));
#endif

        CreateChildWindow();

        {
            Tear = DX12Support::CanTear();
            DXGI_SWAP_CHAIN_DESC1 SCD = {};
            SCD.BufferCount = FrameCount;
            auto Size = Frame->GetSize();
            SCD.Width = Size.Width;
            SCD.Height = Size.Height;
            SCD.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            SCD.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            SCD.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            SCD.SampleDesc.Count = 1;
            SCD.Flags = Tear ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

            Ptr<IDXGISwapChain1> SwapChain;
            VALIDATE(Factory->CreateSwapChainForHwnd(
                CommandQueue.Get(),
                Child,
                &SCD,
                nullptr,
                nullptr,
                &SwapChain
            ));

            VALIDATE(Factory->MakeWindowAssociation(Child, DXGI_MWA_NO_ALT_ENTER));

            VALIDATE(SwapChain.As(&Swap));
        }

        FrameIndex = Swap->GetCurrentBackBufferIndex();

        {
            D3D12_DESCRIPTOR_HEAP_DESC DHD = {};
            DHD.NumDescriptors = FrameCount;
            DHD.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            DHD.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            VALIDATE(Device->CreateDescriptorHeap(&DHD, IID_PPV_ARGS(&RTVHeap)));

            RTVDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        }

        {
            D3D12_CPU_DESCRIPTOR_HANDLE CDH = RTVHeap->GetCPUDescriptorHandleForHeapStart();

            for(U32 I = 0; I < FrameCount; I++)
            {
                VALIDATE(Swap->GetBuffer(I, IID_PPV_ARGS(&RenderTargets[I])));
                Device->CreateRenderTargetView(RenderTargets[I].Get(), nullptr, CDH);
                CDH.ptr += RTVDescriptorSize;

                VALIDATE(Device->CreateCommandAllocator(
                    D3D12_COMMAND_LIST_TYPE_DIRECT,
                    IID_PPV_ARGS(&CommandAllocators[I])
                ));
            }
        }

        {
            D3D12_DESCRIPTOR_HEAP_DESC DHD = {};
            DHD.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            DHD.NumDescriptors = 1;
            DHD.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            VALIDATE(Device->CreateDescriptorHeap(&DHD, IID_PPV_ARGS(&SRVHeap)));
        }
    }

    void DX12::LoadData()
    {
        {
            D3D12_ROOT_SIGNATURE_DESC RSD = {};
            RSD.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

            Ptr<ID3DBlob> Signature;
            Ptr<ID3DBlob> Error;
            VALIDATE(D3D12SerializeRootSignature(&RSD, D3D_ROOT_SIGNATURE_VERSION_1, &Signature, &Error));
            VALIDATE(Device->CreateRootSignature(
                0,
                Signature->GetBufferPointer(),
                Signature->GetBufferSize(),
                IID_PPV_ARGS(&RootSignature)
            ));
        }

        {
            Ptr<ID3DBlob> VertexShader;
            Ptr<ID3DBlob> PixelShader;

            UINT CompileFlags = ShaderOptimizationLevel;

            VALIDATE(D3DCompileFromFile(
                L"C:\\Users\\Elliot\\source\\repos\\RPCS3X\\Volts\\GUI\\Render\\DX12\\shaders.hlsl",
                nullptr,
                nullptr,
                "VSMain",
                "vs_5_0",
                CompileFlags,
                0,
                &VertexShader,
                nullptr
            ));

            VALIDATE(D3DCompileFromFile(
                L"C:\\Users\\Elliot\\source\\repos\\RPCS3X\\Volts\\GUI\\Render\\DX12\\shaders.hlsl",
                nullptr,
                nullptr,
                "PSMain",
                "ps_5_0",
                CompileFlags,
                0,
                &PixelShader,
                nullptr
            ));

            D3D12_INPUT_ELEMENT_DESC IED[] =
            {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
            };

            D3D12_GRAPHICS_PIPELINE_STATE_DESC PSO = {};
            PSO.InputLayout = {IED, _countof(IED) };
            PSO.pRootSignature = RootSignature.Get();
            PSO.VS = CD3DX12_SHADER_BYTECODE(VertexShader.Get());
            PSO.PS = CD3DX12_SHADER_BYTECODE(PixelShader.Get());

            D3D12_RASTERIZER_DESC RD = {};
            RD.FillMode = D3D12_FILL_MODE_SOLID;
            RD.CullMode = D3D12_CULL_MODE_BACK;
            RD.FrontCounterClockwise = false;
            RD.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
            RD.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
            RD.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
            RD.DepthClipEnable = true;
            RD.MultisampleEnable = false;
            RD.AntialiasedLineEnable = false;
            RD.ForcedSampleCount = 0;
            RD.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

            PSO.RasterizerState = RD;

            D3D12_BLEND_DESC BD = {};
            BD.AlphaToCoverageEnable = false;
            BD.IndependentBlendEnable = false;

            D3D12_RENDER_TARGET_BLEND_DESC RTBD =
            {
                false, false,
                D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
                D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
                D3D12_LOGIC_OP_NOOP,
                D3D12_COLOR_WRITE_ENABLE_ALL
            };
            for(U32 I = 0; I < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; I++)
                BD.RenderTarget[I] = RTBD;

            PSO.BlendState = BD;

            PSO.DepthStencilState.DepthEnable = false;
            PSO.DepthStencilState.StencilEnable = false;
            PSO.SampleMask = UINT_MAX;
            PSO.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            PSO.NumRenderTargets = 1;
            PSO.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
            PSO.SampleDesc.Count = 1;
            VALIDATE(Device->CreateGraphicsPipelineState(&PSO, IID_PPV_ARGS(&PipelineState)));
        }

        VALIDATE(Device->CreateCommandList(
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            CommandAllocators[FrameIndex].Get(),
            PipelineState.Get(),
            IID_PPV_ARGS(&CommandList)
        ));

        VALIDATE(CommandList->Close());

        {
            Vertex Tris[] =
            {
                { { 0.f, 0.25f * 2, 0.f }, { 1.f, 0.f, 0.f, 1.f } },
                { { 0.25f, -0.25f * 2, 0.f }, { 0.f, 1.f, 0.f, 1.f } },
                { { -0.25f, -0.25f * 2, 0.f }, { 0.f, 0.f, 1.f, 1.f } }
            };

            const U32 TrisSize = sizeof(Tris);

            VALIDATE(Device->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Buffer(TrisSize),
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&VertexBuffer)
            ));

            Byte* VertexData;
            CD3DX12_RANGE ReadRange(0, 0);
            VALIDATE(VertexBuffer->Map(0, &ReadRange, reinterpret_cast<void**>(&VertexData)));
            memcpy(VertexData, Tris, TrisSize);
            VertexBuffer->Unmap(0, nullptr);

            VertexBufferView.BufferLocation = VertexBuffer->GetGPUVirtualAddress();
            VertexBufferView.StrideInBytes = sizeof(Vertex);
            VertexBufferView.SizeInBytes = TrisSize;
        }

        {
            VALIDATE(Device->CreateFence(
                FenceValues[FrameIndex],
                D3D12_FENCE_FLAG_NONE,
                IID_PPV_ARGS(&Fence)
            ));
            FenceValues[FrameIndex]++;

            FenceEvent = CreateEvent(nullptr, false, false, nullptr);
            if(!FenceEvent)
            {
                VALIDATE(HRESULT_FROM_WIN32(GetLastError()));
            }

            WaitForGPU();
        }
    }

    void DX12::WaitForGPU()
    {
        VALIDATE(CommandQueue->Signal(Fence.Get(), FenceValues[FrameIndex]));
        VALIDATE(Fence->SetEventOnCompletion(FenceValues[FrameIndex], FenceEvent));
        WaitForSingleObjectEx(FenceEvent, INFINITE, false);

        FenceValues[FrameIndex]++;
    }

    void DX12::AdvanceFrame()
    {
        U64 CurrentFenceValue = FenceValues[FrameIndex];
        VALIDATE(CommandQueue->Signal(Fence.Get(), CurrentFenceValue));

        FrameIndex = Swap->GetCurrentBackBufferIndex();

        if(Fence->GetCompletedValue() < FenceValues[FrameIndex])
        {
            VALIDATE(Fence->SetEventOnCompletion(FenceValues[FrameIndex], FenceEvent));
            WaitForSingleObjectEx(FenceEvent, INFINITE, false);
        }

        FenceValues[FrameIndex] = CurrentFenceValue + 1;
    }
}