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
                OutputDebugString("DoneQuit\n");
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

                char Fmt[128] = {};
                _snprintf(Fmt, sizeof(Fmt), "Target %u", I);

                std::wstring W(&Fmt[0], &Fmt[strlen(Fmt)]);
                Targets[I]->SetName(W.c_str());
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

        {
            DX_CHECK(Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE, IID_PPV_ARGS(&BundleAllocator)));
            DX_CHECK(Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_BUNDLE, BundleAllocator.Get(), PipelineState.Get(), IID_PPV_ARGS(&Bundle)));
            Bundle->SetGraphicsRootSignature(RootSignature.Get());
            Bundle->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            Bundle->IASetVertexBuffers(0, 1, &VertexBufferView);
            Bundle->DrawInstanced(3, 1, 0, 0);
            DX_CHECK(Bundle->Close());
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
        OutputDebugString("Render\n");
        PopulateCommandList();

        ID3D12CommandList* Lists[] = { CommandList.Get() };
        Queue->ExecuteCommandLists(_countof(Lists), Lists);

        DX_CHECK(Swap->Present(1, 0));

        MoveToNextFrame();
    }

    void DirectX12::PopulateCommandList()
    {
        OutputDebugString("PopulateCommandList\n");

        DX_CHECK(Allocators[FrameIndex]->Reset());
        DX_CHECK(CommandList->Reset(Allocators[FrameIndex].Get(), PipelineState.Get()));

        CommandList->SetGraphicsRootSignature(RootSignature.Get());
        CommandList->RSSetViewports(1, &Viewport);
        CommandList->RSSetScissorRects(1, &ScissorRect);

        CommandList->ResourceBarrier(1, &DX12::ResourceBarrier::Transition(Targets[FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

        D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle = RTVHeap->GetCPUDescriptorHandleForHeapStart();
        RTVHandle.ptr += FrameIndex * RTVDescriptorSize;
        CommandList->OMSetRenderTargets(1, &RTVHandle, false, nullptr);

        const F32 ClearColour[] = { 0.0f, 0.2f, 0.4f, 1.0f };
        CommandList->ClearRenderTargetView(RTVHandle, ClearColour, 0, nullptr);

        CommandList->ExecuteBundle(Bundle.Get());

        CommandList->ResourceBarrier(1, &DX12::ResourceBarrier::Transition(Targets[FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

        DX_CHECK(CommandList->Close());
    }

    void DirectX12::MoveToNextFrame()
    {
        OutputDebugString("MoveToNextFrame\n");
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
        OutputDebugString("WaitForGPU\n");
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
