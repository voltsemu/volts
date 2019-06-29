#include "Render.h"

#include <comdef.h>

#include <d3dcompiler.h>

namespace Volts::PS3::RSX
{
    using namespace Cthulhu;

    DirectX12::DirectX12()
        // TODO: dont hardcode
        : Viewport({ 0.0f, 0.0f, 500.0f, 500.0f, D3D12_MIN_DEPTH, D3D12_MAX_DEPTH })
    {
        Render::Register(this);
        // get all the things we can use as a render device
        DX12::Factory* Factory = nullptr;
        CreateDXGIFactory1(__uuidof(DX12::Factory), (void**)&Factory);
        DX12::Adapter* Adapt = nullptr;
        for(U32 I = 0; Factory->EnumAdapters1(I, &Adapt) != DXGI_ERROR_NOT_FOUND; I++)
            RenderDevices.Append(Adapt);

        SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    }

    LRESULT CALLBACK Proc(HWND Wnd, UINT M, WPARAM W, LPARAM L)
    {
        auto* DXWindow = (DirectX12*)GetWindowLongPtr(Wnd, GWLP_USERDATA);

        switch(M)
        {
            case WM_PAINT:
                DXWindow->Render();
                break;
            case WM_CREATE:
            {
                auto* Parent = (DirectX12*)((LPCREATESTRUCT)L)->lpCreateParams;
                SetWindowLongPtr(Wnd, GWLP_USERDATA, (LONG_PTR)Parent);
                break;
            }
            case WM_DESTROY:
            {
                DXWindow->DeInit();
                PostQuitMessage(0);
                break;
            }
            default:
                return DefWindowProc(Wnd, M, W, L);
        }

        return 0;
    }

    InitError DirectX12::Init(RenderDevice* Dev)
    {
        // TODO: dont hardcode
        Frame.Width = 500;
        Frame.Height = 500;
        Frame.X = 500;
        Frame.Y = 500;
        Frame.InputHandle(Proc);
        Frame.Title = "DirectX12 Volts";
        Frame.Create(this);

        GetWindowRect(Frame.GetHandle(), &ScissorRect);

#if VDXDEBUG
        EnableDebugger();
        UINT FactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#else
        UINT FactoryFlags = 0;
#endif

        // create factory
        DX12::ComPtr<DX12::Factory> Factory;
        DX_CHECK(CreateDXGIFactory2(FactoryFlags, IID_PPV_ARGS(&Factory)));


        // create device from adapter
        auto* DXDevice = (DX12::DX12Device*)Dev;
        DX_CHECK(D3D12CreateDevice(DXDevice->GetAdapter(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&Device)));


        // create command queue
        {
            D3D12_COMMAND_QUEUE_DESC Desc = {};
            Desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
            Desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

            DX_CHECK(Device->CreateCommandQueue(&Desc, IID_PPV_ARGS(&Queue)));
        }

        // create swap chain
        {
            DXGI_SWAP_CHAIN_DESC1 Desc = {};
            Desc.BufferCount = FrameCount;
            // TODO: dont hardcode
            Desc.Width = 500;
            Desc.Height = 500;
            Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            Desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            Desc.SampleDesc.Count = 1;

            DX12::ComPtr<DX12::SwapChain1> Swap1;
            DX_CHECK(Factory->CreateSwapChainForHwnd(
                Queue.Get(),
                Frame.GetHandle(),
                &Desc,
                nullptr,
                nullptr,
                &Swap1
            ));

            // TODO: support fullscreen transitions and also borderless fullscreen
            DX_CHECK(Factory->MakeWindowAssociation(Frame.GetHandle(), DXGI_MWA_NO_ALT_ENTER));

            DX_CHECK(Swap1.As(&Swap));
            FrameIndex = Swap->GetCurrentBackBufferIndex();
        }

        // create descriptor heaps
        {
            D3D12_DESCRIPTOR_HEAP_DESC Desc = {};
            Desc.NumDescriptors = FrameCount;
            Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            DX_CHECK(Device->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&RTVHeap)));

            RTVDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        }

        // create frame resources
        {
            D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle = RTVHeap->GetCPUDescriptorHandleForHeapStart();

            for(U32 I = 0; I < FrameCount; I++)
            {
                DX_CHECK(Swap->GetBuffer(I, IID_PPV_ARGS(&Targets[I])));
                Device->CreateRenderTargetView(Targets[I].Get(), nullptr, RTVHandle);
                RTVHandle.ptr += RTVDescriptorSize;

                DX_CHECK(Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&Allocators[I])));
            }
        }

        {
            D3D12_ROOT_SIGNATURE_DESC Desc = {};
            Desc.NumParameters = 0;
            Desc.pParameters = nullptr;
            Desc.NumStaticSamplers = 0;
            Desc.pStaticSamplers = nullptr;
            Desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

            DX12::ComPtr<DX12::Blob> Signature;
            DX12::ComPtr<DX12::Blob> Error;

            DX_CHECK(D3D12SerializeRootSignature(&Desc, D3D_ROOT_SIGNATURE_VERSION_1, &Signature, &Error));
            DX_CHECK(Device->CreateRootSignature(0, Signature->GetBufferPointer(), Signature->GetBufferSize(), IID_PPV_ARGS(&RootSignature)));
        }

        {
            DX12::ComPtr<DX12::Blob> VertexShader;
            DX12::ComPtr<DX12::Blob> PixelShader;

#if VDXDEBUG
            UINT CompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
            UINT CompileFlags = 0;
#endif

            DX_CHECK(D3DCompileFromFile(L"C:\\Users\\Elliot\\source\\repos\\RPCS3X\\Volts\\PS3\\VM\\Render\\DirectX12\\shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", CompileFlags, 0, &VertexShader, nullptr));
            DX_CHECK(D3DCompileFromFile(L"C:\\Users\\Elliot\\source\\repos\\RPCS3X\\Volts\\PS3\\VM\\Render\\DirectX12\\shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", CompileFlags, 0, &PixelShader, nullptr));

            D3D12_INPUT_ELEMENT_DESC Descs[] =
            {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
            };

            D3D12_GRAPHICS_PIPELINE_STATE_DESC PSDesc = {};
            PSDesc.InputLayout = { Descs, _countof(Descs) };
            PSDesc.pRootSignature = RootSignature.Get();
            PSDesc.VS = { VertexShader->GetBufferPointer(), VertexShader->GetBufferSize() };
            PSDesc.PS = { PixelShader->GetBufferPointer(), PixelShader->GetBufferSize() };
            PSDesc.RasterizerState = DX12::RasterDesc();
            PSDesc.BlendState = DX12::BlendDesc();
            PSDesc.DepthStencilState.DepthEnable = false;
            PSDesc.DepthStencilState.StencilEnable = false;
            PSDesc.SampleMask = UINT_MAX;
            PSDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            PSDesc.NumRenderTargets = 1;
            PSDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
            PSDesc.SampleDesc.Count = 1;

            DX_CHECK(Device->CreateGraphicsPipelineState(&PSDesc, IID_PPV_ARGS(&PipelineState)));
        }

        DX_CHECK(Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, Allocators[FrameIndex].Get(), PipelineState.Get(), IID_PPV_ARGS(&CommandList)));

        DX_CHECK(CommandList->Close());

        {
            DX12::Vertex Triangle[] =
            {
                { { 0.0f, 0.25f * Frame.AspectRatio(), 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
                { { 0.25f, -0.25f * Frame.AspectRatio(), 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
                { { -0.25f, -0.25f * Frame.AspectRatio(), 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
            };

            const UINT BufferSize = sizeof(Triangle);

            DX_CHECK(Device->CreateCommittedResource(
                &DX12::HeapProps(D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAG_NONE,
                &DX12::ResourceDesc::Buffer(BufferSize),
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&VertexBuffer)
            ));

            UINT8* VertexData;
            D3D12_RANGE ReadRange = { 0, 0 };
            DX_CHECK(VertexBuffer->Map(0, &ReadRange, reinterpret_cast<void**>(&VertexData)));
            memcpy(VertexData, Triangle, sizeof(Triangle));
            VertexBuffer->Unmap(0, nullptr);

            VertexBufferView.BufferLocation = VertexBuffer->GetGPUVirtualAddress();
            VertexBufferView.StrideInBytes = sizeof(DX12::Vertex);
            VertexBufferView.SizeInBytes = BufferSize;
        }

        {
            DX_CHECK(Device->CreateFence(FenceValues[FrameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)));
            FenceValues[FrameIndex]++;

            FenceEvent = CreateEvent(nullptr, false, false, nullptr);
            if(FenceEvent == nullptr)
            {
                DX_CHECK(HRESULT_FROM_WIN32(GetLastError()));
            }

            WaitForGPU();
        }

        Frame.Show();

        while(Frame.ShouldStayOpen())
            Frame.PollEvents();

        return InitError::Ok;
    }

    void DirectX12::DeInit()
    {
        WaitForGPU();
        CloseHandle(FenceEvent);
    }

    bool DirectX12::Supported() const
    {
        return false;
    }

    RenderDevice* DirectX12::Devices(unsigned& Count) const
    {
        Count = RenderDevices.Len();
        return RenderDevices.Data();
    }

    void DirectX12::Render()
    {
        PopulateCommandList();

        ID3D12CommandList* Lists[] = { CommandList.Get() };
        Queue->ExecuteCommandLists(_countof(Lists), Lists);

        DX_CHECK(Swap->Present(1, 0));

        MoveToNextFrame();
    }

    void DirectX12::PopulateCommandList()
    {
        OutputDebugString("Before Reset\n");
        DX_CHECK(Allocators[FrameIndex]->Reset());
        DX_CHECK(CommandList->Reset(Allocators[FrameIndex].Get(), PipelineState.Get()));

        CommandList->SetGraphicsRootSignature(RootSignature.Get());
        CommandList->RSSetViewports(1, &Viewport);
        CommandList->RSSetScissorRects(1, &ScissorRect);

        OutputDebugString("Before Transition\n");
        CommandList->ResourceBarrier(1, &DX12::ResourceBarrier::Transition(Targets[FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

        D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle = RTVHeap->GetCPUDescriptorHandleForHeapStart();
        CommandList->OMSetRenderTargets(1, &RTVHandle, false, nullptr);

        const F32 ClearColour[] = { 0.0f, 0.2f, 0.4f, 1.0f };
        CommandList->ClearRenderTargetView(RTVHandle, ClearColour, 0, nullptr);
        CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        CommandList->IASetVertexBuffers(0, 1, &VertexBufferView);
        CommandList->DrawInstanced(3, 1, 0, 0);

        CommandList->ResourceBarrier(1, &DX12::ResourceBarrier::Transition(Targets[FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
        OutputDebugString("After Transition\n");

        DX_CHECK(CommandList->Close());
        OutputDebugString("Close\n");
    }

    void DirectX12::MoveToNextFrame()
    {
        const UINT64 CurrentFenceValue = FenceValues[FrameIndex];
        DX_CHECK(Queue->Signal(Fence.Get(), CurrentFenceValue));

        FrameIndex = Swap->GetCurrentBackBufferIndex();

        if(Fence->GetCompletedValue() < FenceValues[FrameIndex])
        {
            DX_CHECK(Fence->SetEventOnCompletion(FenceValues[FrameIndex], FenceEvent));
            WaitForSingleObjectEx(FenceEvent, INFINITE, false);
        }

        FenceValues[FrameIndex] = CurrentFenceValue + 1;
    }

    void DirectX12::WaitForGPU()
    {
        DX_CHECK(Queue->Signal(Fence.Get(), FenceValues[FrameIndex]));

        DX_CHECK(Fence->SetEventOnCompletion(FenceValues[FrameIndex], FenceEvent));
        WaitForSingleObjectEx(FenceEvent, INFINITE, false);

        FenceValues[FrameIndex]++;
    }

#if VDXDEBUG
    void DirectX12::EnableDebugger()
    {
        if(DX12::ComPtr<DX12::Debug> Debugger; SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&Debugger))))
            Debugger->EnableDebugLayer();
    }
#endif

    static DirectX12* DX12Singleton = new DirectX12();
}

#if 0
        // TODO: dont hardcode this
        auto DXDevice = (DX12::DX12Device*)Dev;

#if VDXDEBUG
        // if we're in debug mode enable the debug layer
        EnableDebugger();
#endif

        TearingSupport = DX12::CanTear();

        DX12::ComPtr<DX12::Factory> Factory;

#if VDXDEBUG
        DX_CHECK(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&Factory)));
#else
        DX_CHECK(CreateDXGIFactory2(0, IID_PPV_ARGS(&Factory)));
#endif

        // create device
        DX_CHECK(D3D12CreateDevice(DXDevice->GetAdapter(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&CurrentDevice)));

#if VDXDEBUG
        DX12::ComPtr<ID3D12InfoQueue> InfoQueue;
        if(SUCCEEDED(CurrentDevice.As(&InfoQueue)))
        {
            InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
            InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
            InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
            // Suppress whole categories of messages
            //D3D12_MESSAGE_CATEGORY Categories[] = {};

            // Suppress messages based on their severity level
            D3D12_MESSAGE_SEVERITY Severities[] =
            {
                D3D12_MESSAGE_SEVERITY_INFO
            };

            // Suppress individual messages by their ID
            D3D12_MESSAGE_ID DenyIds[] = {
                D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
                D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
                D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
            };

            D3D12_INFO_QUEUE_FILTER NewFilter = {};
            //NewFilter.DenyList.NumCategories = _countof(Categories);
            //NewFilter.DenyList.pCategoryList = Categories;
            NewFilter.DenyList.NumSeverities = _countof(Severities);
            NewFilter.DenyList.pSeverityList = Severities;
            NewFilter.DenyList.NumIDs = _countof(DenyIds);
            NewFilter.DenyList.pIDList = DenyIds;

            DX_CHECK(InfoQueue->PushStorageFilter(&NewFilter));
        }
#endif

        // create command queue
        {
            D3D12_COMMAND_QUEUE_DESC Desc = {};
            Desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
            Desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
            Desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            Desc.NodeMask = 0;

            DX_CHECK(CurrentDevice->CreateCommandQueue(&Desc, IID_PPV_ARGS(&Queue)));
        }

        // create swap chain
        DXGI_SWAP_CHAIN_DESC1 SDesc = {};
        SDesc.BufferCount = SwapFrames;
        SDesc.Width = 500;
        SDesc.Height = 500;
        SDesc.Stereo = false;
        SDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        SDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        SDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        SDesc.Scaling = DXGI_SCALING_STRETCH;
        SDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        SDesc.SampleDesc = { 1, 0 };
        SDesc.Flags = TearingSupport ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

        DX12::ComPtr<DX12::SwapChain1> Swap1;

        DX_CHECK(Factory->CreateSwapChainForHwnd(
            Queue.Get(),
            Frame.GetHandle(),
            &SDesc,
            nullptr,
            nullptr,
            &Swap1
        ));

        DX_CHECK(Swap1.As(&Swap));

        DX_CHECK(Factory->MakeWindowAssociation(Frame.GetHandle(), DXGI_MWA_NO_ALT_ENTER));

        // get current buffer index
        CurrentBackBuffer = Swap->GetCurrentBackBufferIndex();

        // create descriptor heap
        {
            D3D12_DESCRIPTOR_HEAP_DESC HDesc = {};
            HDesc.NumDescriptors = SwapFrames;
            HDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            HDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            DX_CHECK(CurrentDevice->CreateDescriptorHeap(&HDesc, IID_PPV_ARGS(&Heap)));
        }

        // get descriptor size
        RTVSize = CurrentDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        // create allocators
        for(U32 I = 0; I < SwapFrames; I++)
        {
            DX_CHECK(CurrentDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&Allocators[I])));
        }

        // create command list
        DX_CHECK(CurrentDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, Allocators[CurrentBackBuffer].Get(), nullptr, IID_PPV_ARGS(&CommandList)));

        // create fence
        CurrentDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence));

        // create fence event handle
        FenceEvent = CreateEvent(nullptr, false, false, nullptr);

        // we're done initializing now
        Initialized = true;

        // show the window
        Frame.Show();

        // message loop
        while(Frame.ShouldStayOpen())
            Frame.PollEvents();

        // finish all commands before closing
        Flush();

        // close
        CloseHandle(FenceEvent);
#endif
#if 0

    void DirectX12::UpdateRTV()
    {
        D3D12_CPU_DESCRIPTOR_HANDLE RHandle = Heap->GetCPUDescriptorHandleForHeapStart();

        for(Cthulhu::U32 I = 0; I < SwapFrames; I++)
        {
            DX_CHECK(Swap->GetBuffer(I, IID_PPV_ARGS(&Buffers[I])));
            CurrentDevice->CreateRenderTargetView(Buffers[I].Get(), nullptr, RHandle);
            RHandle.ptr += RTVSize;
        }
    }

    void DirectX12::Update()
    {
        auto Before = time::high_resolution_clock::now();
        Stats.FrameCount++;

        auto After = time::high_resolution_clock::now();

        auto Delta = (After - Before);
        Stats.LastFrame = Delta.count();

        Stats.ElapsedTime += Delta.count();

        if(Stats.ElapsedTime > 1.0)
        {
            ATL::CString Str;
            Str.Format("FPS: %f\n", (Stats.ElapsedTime / Stats.FrameCount));
            OutputDebugString(Str);
        }

        CommandList->Close();
    }

    void DirectX12::Render()
    {
        auto Alloc = Allocators[CurrentBackBuffer];
        auto Buffer = Buffers[CurrentBackBuffer];

        Alloc->Reset();
        // TODO: it goes boom here
        CommandList->Reset(Alloc.Get(), nullptr);

        {
            D3D12_RESOURCE_BARRIER Barrier = {};
            Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            Barrier.Transition.pResource = Buffer.Get();
            Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
            Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
            Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

            CommandList->ResourceBarrier(1, &Barrier);

            FLOAT Clear[] = { 0.4f, 0.6f, 0.9f, 1.0f };
            D3D12_CPU_DESCRIPTOR_HANDLE RTV = Heap->GetCPUDescriptorHandleForHeapStart();
            RTV.ptr += CurrentBackBuffer * RTVSize;

            CommandList->ClearRenderTargetView(RTV, Clear, 0, nullptr);
        }

        {
            D3D12_RESOURCE_BARRIER Barrier = {};
            Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            Barrier.Transition.pResource = Buffer.Get();
            Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
            Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
            Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

            CommandList->ResourceBarrier(1, &Barrier);

            DX_CHECK(CommandList->Close());

            ID3D12CommandList* const Lists[] = { CommandList.Get() };
            Queue->ExecuteCommandLists(_countof(Lists), Lists);
            UINT SyncInterval = VSync ? 1 : 0;
            UINT Flags = TearingSupport && !VSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
            DX_CHECK(Swap->Present(SyncInterval, Flags));

            FenceValues[CurrentBackBuffer] = Signal(FenceVal);
            CurrentBackBuffer = Swap->GetCurrentBackBufferIndex();

            WaitForFenceValue(FenceValues[CurrentBackBuffer]);
        }

        Flush();
    }

    void DirectX12::Resize(RECT NewSize)
    {
        U32 Width = NewSize.right - NewSize.left;
        U32 Height = NewSize.bottom - NewSize.top;

        U32 OldWidth = WindowRect.right - WindowRect.left;
        U32 OldHeight = WindowRect.bottom - WindowRect.top;

        if(Width != OldWidth && Height != OldHeight)
        {
            auto ClientWidth = Math::Max<U32>(Width, 1U);
            auto ClientHeight = Math::Max<U32>(Height, 1U);

            Flush();

            for(U32 I = 0; I < SwapFrames; I++)
            {
                Buffers[I].Reset();
                FenceValues[I] = FenceValues[CurrentBackBuffer];
            }

            DXGI_SWAP_CHAIN_DESC SwapDesc = {};
            DX_CHECK(Swap->GetDesc(&SwapDesc));
            DX_CHECK(Swap->ResizeBuffers(
                SwapFrames,
                ClientWidth,
                ClientHeight,
                DXGI_FORMAT_UNKNOWN,
                SwapDesc.Flags
            ));

            CurrentBackBuffer = Swap->GetCurrentBackBufferIndex();

            UpdateRTV();
        }
    }

    Cthulhu::U64 DirectX12::Signal(Cthulhu::U64& Val)
    {
        U64 SignalVal = ++Val;
        DX_CHECK(Queue->Signal(Fence.Get(), SignalVal));

        return SignalVal;
    }

    void DirectX12::WaitForFenceValue(Cthulhu::U64 Val, Cthulhu::U64 Duration)
    {
        if(Fence->GetCompletedValue() < Val)
        {
            DX_CHECK(Fence->SetEventOnCompletion(Val, FenceEvent));
            WaitForSingleObject(FenceEvent, (DWORD)Duration);
        }
    }

    void DirectX12::Flush()
    {
        U64 SignalVal = Signal(FenceVal);
        WaitForFenceValue(FenceVal);
    }

#if VDXDEBUG
    void DirectX12::EnableDebugger()
    {
        DX_CHECK(D3D12GetDebugInterface(IID_PPV_ARGS(&Debugger)));
        Debugger->EnableDebugLayer();
    }
#endif

#endif