#pragma once

#include "Render/Render.h"

#include "Support.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <wrl.h>
#include <shellapi.h>

namespace Volts::RSX
{
    template<typename T>
    using Ptr = Microsoft::WRL::ComPtr<T>;

    struct DX12 : Render
    {
        virtual ~DX12() {}
        virtual void Attach(GUI::Frame* Handle) override;
        virtual void Detach() override;
        virtual const String Name() const override { return "DX12"; }
        virtual const String Description() const override { return "DirectX12"; }

        virtual void Resize(GUI::Size NewSize) override;

        virtual void BeginRender() override;
        virtual void PresentRender() override;

        virtual void Windowed() override;
        virtual void Borderless() override;
        virtual void Fullscreen() override;

    private:

        struct Vertex
        {
            F32 Pos[3];
            F32 Colour[4];
        };

        void LoadPipeline();
        void LoadData();
        void PopulateCommandList();
        void WaitForGPU();
        void AdvanceFrame();

        static constexpr U32 FrameCount = 2;

        // Pipeline state objects
        D3D12_VIEWPORT Viewport;
        D3D12_RECT Scissor;

        Ptr<IDXGISwapChain3> Swap;
        Ptr<ID3D12Device> Device;
        Ptr<ID3D12CommandQueue> CommandQueue;

        Ptr<ID3D12Resource> RenderTargets[FrameCount];
        Ptr<ID3D12CommandAllocator> CommandAllocators[FrameCount];

        Ptr<ID3D12RootSignature> RootSignature;
        Ptr<ID3D12PipelineState> PipelineState;
        Ptr<ID3D12GraphicsCommandList> CommandList;
        Ptr<ID3D12DescriptorHeap> RTVHeap;
        U32 RTVDescriptorSize;

        Ptr<ID3D12DescriptorHeap> SRVHeap;

        Ptr<ID3D12Resource> VertexBuffer;
        D3D12_VERTEX_BUFFER_VIEW VertexBufferView;

#if VDXDEBUG
        Ptr<ID3D12InfoQueue> DebugQueue;
#endif

        // Synchronization data
        U32 FrameIndex;
        HANDLE FenceEvent;
        Ptr<ID3D12Fence> Fence;
        U64 FenceValues[FrameCount];

        // extra data
        bool Tear;

        // External data
        GUI::Frame* Frame;
    };
}