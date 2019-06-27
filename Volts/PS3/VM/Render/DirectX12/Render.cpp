#include "Render.h"

#include <comdef.h>

namespace Volts::PS3::RSX
{
    using namespace Cthulhu;

    DirectX12::DirectX12()
    {
        Render::Register(this);
        // get all the things we can use as a render device
        DX12::Factory* Factory = nullptr;
        CreateDXGIFactory1(__uuidof(DX12::Factory), (void**)&Factory);
        DX12::Adapter* Adapt = nullptr;
        for(U32 I = 0; Factory->EnumAdapters1(I, &Adapt) != DXGI_ERROR_NOT_FOUND; I++)
            RenderDevices.Append(Adapt);

        CanTear = DX12::TearingSupport();
        SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    }

    LRESULT CALLBACK Proc(HWND Wnd, UINT M, WPARAM W, LPARAM L)
    {
        switch(M)
        {
            case WM_DESTROY:
                // TODO: this isnt right...
                exit(0);
            case WM_CREATE:
                break;
            default:
                return DefWindowProc(Wnd, M, W, L);
        }
        return 0;
    }

    InitError DirectX12::Init(RenderDevice* Dev)
    {
        Frame.SetWidth(500);
        Frame.SetHeight(500);
        Frame.SetX(500);
        Frame.SetY(500);
        Frame.InputHandle(Proc);
        Frame.SetTitle("DirectX12 Volts");
        Frame.Create(this);

        // TODO: dont hardcode this
        auto DXDevice = (DX12::DX12Device*)Dev;

#if VDXDEBUG
        DX_CHECK(D3D12GetDebugInterface(IID_PPV_ARGS(&Debugger)));
        Debugger->EnableDebugLayer();
#endif

        DX12::ComPtr<DX12::Factory> Factory;

#if VDXDEBUG
        DX_CHECK(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&Factory)));
#else
        DX_CHECK(CreateDXGIFactory2(0, IID_PPV_ARGS(&Factory)));
#endif

        DX_CHECK(D3D12CreateDevice(DXDevice->GetAdapter(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&CurrentDevice)));

        D3D12_COMMAND_QUEUE_DESC QDesc = {};
        QDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        QDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

        DX_CHECK(CurrentDevice->CreateCommandQueue(&QDesc, IID_PPV_ARGS(&Queue)));

        DXGI_SWAP_CHAIN_DESC1 SDesc = {};
        SDesc.BufferCount = SwapFrames;
        SDesc.Width = 500;
        SDesc.Height = 500;
        SDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        SDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        SDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        SDesc.SampleDesc.Count = 1;

        DX12::ComPtr<DX12::SwapChain1> Swap1;

        DX_CHECK(Factory->CreateSwapChainForHwnd(Queue.Get(), Frame.GetHandle(), &SDesc, nullptr, nullptr, &Swap1));

        DX_CHECK(Swap1.As(&Swap));

        DX_CHECK(Factory->MakeWindowAssociation(Frame.GetHandle(), DXGI_MWA_NO_ALT_ENTER));

        CurrentBackBuffer = Swap->GetCurrentBackBufferIndex();

        {
            D3D12_DESCRIPTOR_HEAP_DESC HDesc = {};
            HDesc.NumDescriptors = SwapFrames;
            HDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            HDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            DX_CHECK(CurrentDevice->CreateDescriptorHeap(&HDesc, IID_PPV_ARGS(&Heap)));

            RTVSize = CurrentDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        }

        {
            D3D12_CPU_DESCRIPTOR_HANDLE RHandle = Heap->GetCPUDescriptorHandleForHeapStart();

            for(Cthulhu::U32 I = 0; I < SwapFrames; I++)
            {
                DX_CHECK(Swap->GetBuffer(I, IID_PPV_ARGS(&Buffers[I])));
                CurrentDevice->CreateRenderTargetView(Buffers[I].Get(), nullptr, RHandle);
                RHandle.ptr += RTVSize;
            }
        }

        CurrentDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&Allocator));

        //SetupDevice(DXDevice->GetAdapter());

        //SetupSwapChain();

        //CurrentBackBuffer = Swap->GetCurrentBackBufferIndex();

        //SetupHeap();


        while(Frame.ShouldStayOpen())
            Frame.PollEvents();

        return InitError::Ok;
    }

    void DirectX12::DeInit()
    {

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

    void DirectX12::SetupSwapChain()
    {
        DX12::ComPtr<DX12::Factory> Factory;

#if VDXDEBUG
        DX_CHECK(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&Factory)));
#else
        DX_CHECK(CreateDXGIFactory2(0, IID_PPV_ARGS(&Factory)));
#endif
        DX12::SwapDescriptor Desc = {};

        // TODO: this might need to be changed
        Desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        Desc.SampleDesc = { 1, 0 };
        Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

        // TODO: might need to be changed
        Desc.BufferCount = SwapFrames;
        Desc.Windowed = true;
        Desc.OutputWindow = Frame.GetHandle();
        Desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        Desc.Flags = CanTear ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

#if VDXDEBUG
        DX_CHECK(D3D12GetDebugInterface(IID_PPV_ARGS(&Debugger)));
        Debugger->EnableDebugLayer();
#endif
        Queue = DX12::CreateCommandQueue(CurrentDevice);

        DX_CHECK(CurrentDevice->GetDeviceRemovedReason());

        DX12::ComPtr<DX12::SwapChain1> Swap1;
        CurrentDevice.As(&Swap1);
        DX_CHECK(Factory->CreateSwapChain(
            Queue.Get(),
            &Desc,
            (IDXGISwapChain**)Swap1.GetAddressOf()
        ));

        DX_CHECK(Factory->MakeWindowAssociation(Frame.GetHandle(), DXGI_MWA_NO_ALT_ENTER));
        Swap1.As(&Swap);
    }

    void DirectX12::SetupHeap()
    {
        DX12::HeapDescriptor Desc = {};

        // TODO: might need to be changed
        Desc.NumDescriptors = SwapFrames;

        // TODO: this probably shouldnt be hardcoded
        Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

        DX_CHECK(CurrentDevice->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&Heap)));
    }

    void DirectX12::SetupDevice(DX12::Adapter* Adapt)
    {
        // TOOD: is there a way that means we dont have to create the device twice?
        DX_CHECK(D3D12CreateDevice(Adapt, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&CurrentDevice)));
#if VDXDEBUG
        if(SUCCEEDED(CurrentDevice.As(&DebugQueue)))
        {
            DebugQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
            DebugQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
            DebugQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

            D3D12_MESSAGE_SEVERITY Severities[] = { D3D12_MESSAGE_SEVERITY_INFO };

            D3D12_MESSAGE_ID Silenced[] = {
                D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
                D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
                D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
            };

            D3D12_INFO_QUEUE_FILTER Filter = {};
            Filter.DenyList.NumSeverities = _countof(Severities);
            Filter.DenyList.pSeverityList = Severities;
            Filter.DenyList.NumIDs = _countof(Silenced);
            Filter.DenyList.pIDList = Silenced;

            DebugQueue->PushStorageFilter(&Filter);
        }
#endif

        DX_CHECK(D3D12CreateDevice(Adapt, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&CurrentDevice)));
    }

    void DirectX12::UpdateRTV()
    {
        // TODO: this shouldnt be hardcoded here
        RTVSize = CurrentDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle(Heap->GetCPUDescriptorHandleForHeapStart());

        for(Cthulhu::U32 I = 0; I < SwapFrames; I++)
        {
            DX12::ComPtr<DX12::Resource> Buffer;
            DX_CHECK(Swap->GetBuffer(I, IID_PPV_ARGS(&Buffer)));

            CurrentDevice->CreateRenderTargetView(Buffer.Get(), nullptr, RTVHandle);

            Buffers[I] = Buffer;

            RTVHandle.ptr += RTVSize;
        }
    }

    DX12::ComPtr<DX12::Allocator> DirectX12::CreateAllocator()
    {
        DX12::ComPtr<DX12::Allocator> Alloc;
        DX_CHECK(CurrentDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&Alloc)));

        return Alloc;
    }

    void DirectX12::SetupList(DX12::Allocator* Alloc)
    {
        DX_CHECK(CurrentDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, Alloc, nullptr, IID_PPV_ARGS(&CommandList)));
        CommandList->Close();
    }

    void DirectX12::SetupFence()
    {
        DX_CHECK(CurrentDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)));
    }

    void DirectX12::SetupFenceHandle()
    {
        FenceEvent = CreateEventA(nullptr, false, false, nullptr);
        // TODO: DX_CHECK to see if it fails
    }

    Cthulhu::U64 DirectX12::Signal(Cthulhu::U64& FenceValue)
    {
        Cthulhu::U64 SignalValue = FenceValue++;
        Queue->Signal(Fence.Get(), SignalValue);

        return SignalValue;
    }

    void DirectX12::WaitForValue(Cthulhu::U64 Value)
    {
        if(Fence->GetCompletedValue() < Value)
        {
            Fence->SetEventOnCompletion(Value, FenceEvent);
            WaitForSingleObject(FenceEvent, static_cast<DWORD>(Value));
        }
    }

    void DirectX12::Flush(Cthulhu::U64& FenceValue)
    {
        Cthulhu::U64 SignalValue = Signal(FenceValue);
        WaitForValue(SignalValue);
    }

    void DirectX12::Update()
    {
        FrameCounter++;
        static Cthulhu::F64 ElapsedTime = 0.0;
#if VDXDEBUG
        OutputDebugString("Name Jeff");
#endif
    }
#if 0
    void DirectX12::Render()
    {
        auto Alloc = Allocators[CurrentBackBuffer];
        auto Buffer = Buffers[CurrentBackBuffer];

        Alloc->Reset();
        CommandList->Reset(Alloc.Get(), nullptr);

        {
            D3D12_RESOURCE_BARRIER Bar = {};
            Bar.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            Bar.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            Bar.Transition.pResource = Buffer.Get();
            Bar.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
            Bar.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
            Bar.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

            CommandList->ResourceBarrier(1, &Bar);

            float Clear[] = { 0.4f, 0.6f, 0.9f, 1.0f };
            D3D12_CPU_DESCRIPTOR_HANDLE RTV = Heap->GetCPUDescriptorHandleForHeapStart();
            RTV.ptr += (CurrentBackBuffer * RTVSize);

            CommandList->ClearRenderTargetView(RTV, Clear, 0, nullptr);
        }

        {
            D3D12_RESOURCE_BARRIER Bar = {};
            Bar.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            Bar.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            Bar.Transition.pResource = Buffer.Get();
            Bar.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
            Bar.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
            Bar.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

            CommandList->ResourceBarrier(1, &Bar);

            CommandList->Close();

            DX12::CommandList* const Lists[] = { CommandList.Get() };

            Queue->ExecuteCommandLists(_countof(Lists), (ID3D12CommandList* const*)Lists);

            UINT Flags = CanTear && !VSync ? DXGI_PRESENT_ALLOW_TEARING : 0;

            Swap->Present(VSync ? 1 : 0, Flags);
            FenceValues[CurrentBackBuffer] = Signal(FenceVal);

            CurrentBackBuffer = Swap->GetCurrentBackBufferIndex();

            WaitForValue(FenceValues[CurrentBackBuffer]);
        }
    }
#endif
    static DirectX12* DX12Singleton = new DirectX12();
}
