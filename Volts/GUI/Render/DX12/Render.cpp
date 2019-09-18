#include "Render.h"
#include <vector>

#include "Frame.h"

#include "Support.h"

#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_win32.h"
#include "imgui/examples/imgui_impl_dx12.h"

namespace Volts::RSX
{
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

        ImGui_ImplWin32_Init(Frame->Handle);
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
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();

        CloseHandle(FenceEvent);
    }

    void DX12::PopulateCommandList()
    {
        VALIDATE(CommandAllocators[FrameIndex]->Reset());
        VALIDATE(CommandList->Reset(CommandAllocators[FrameIndex].Get(), PipelineState.Get()));

        CommandList->SetGraphicsRootSignature(RootSignature.Get());
        CommandList->RSSetViewports(1, &Viewport);
        CommandList->RSSetScissorRects(1, &Scissor);

        D3D12_RESOURCE_BARRIER Barrier = {};
        Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        Barrier.Transition = {
            RenderTargets[FrameIndex].Get(),
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET
        };

        CommandList->ResourceBarrier(1, &Barrier);

        D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle = {};
        RTVHandle.ptr = RTVHeap->GetCPUDescriptorHandleForHeapStart().ptr + (FrameIndex * RTVDescriptorSize);
        CommandList->OMSetRenderTargets(1, &RTVHandle, false, nullptr);
        CommandList->SetDescriptorHeaps(1, &SRVHeap);

        // render stuff in here

        ImGui::Render();
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), CommandList.Get());

        D3D12_RESOURCE_BARRIER Transition = {};
        Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        Barrier.Transition = {
            RenderTargets[FrameIndex].Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT
        };

        CommandList->ResourceBarrier(1, &Transition);

        // stop rendering here

        VALIDATE(CommandList->Close());
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

        VALIDATE(Swap->Present(1, 0));

        AdvanceFrame();
    }

    void DX12::LoadPipeline()
    {
        UINT FactoryFlags = 0;

        Ptr<IDXGIFactory4> Factory;
        VALIDATE(CreateDXGIFactory2(FactoryFlags, IID_PPV_ARGS(&Factory)));

        Ptr<IDXGIAdapter1> Adapter;
        std::vector<Ptr<IDXGIAdapter1>> Adapters;

        for(U32 I = 0; Factory->EnumAdapters1(I, &Adapter) != DXGI_ERROR_NOT_FOUND; I++)
        {
            Adapters.push_back(Adapter);
        }

        VALIDATE(D3D12CreateDevice(
            Adapter.Get(),
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

        {
            DXGI_SWAP_CHAIN_DESC1 SCD = {};
            SCD.BufferCount = FrameCount;
            auto Size = Frame->GetSize();
            SCD.Width = Size.Width;
            SCD.Height = Size.Height;
            SCD.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            SCD.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            SCD.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            SCD.SampleDesc.Count = 1;
            SCD.Flags = DX12Support::CanTear() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

            Ptr<IDXGISwapChain1> SwapChain;
            VALIDATE(Factory->CreateSwapChainForHwnd(
                CommandQueue.Get(),
                Frame->Handle,
                &SCD,
                nullptr,
                nullptr,
                &SwapChain
            ));

            VALIDATE(Factory->MakeWindowAssociation(Frame->Handle, DXGI_MWA_NO_ALT_ENTER));

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

        VALIDATE(Device->CreateCommandList(
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            CommandAllocators[FrameIndex].Get(),
            PipelineState.Get(),
            IID_PPV_ARGS(&CommandList)
        ));

        VALIDATE(CommandList->Close());

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