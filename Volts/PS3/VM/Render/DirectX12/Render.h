#pragma once

#include "DX12Support.h"
#include "PS3/VM/Render/Frame.h"

#include <Core/Collections/Array.h>
#include <Core/Math/Limits.h>

namespace Volts::PS3::RSX
{
    struct DirectX12 : Render
    {
        DirectX12();
        virtual ~DirectX12() override {}

        virtual InitError Init(RenderDevice* Device) override;
        virtual void DeInit() override;
        virtual bool Supported() const override;

        virtual RenderDevice* Devices(unsigned& Count) const override;

        virtual const char* Name() const override { return "DirectX12"; }
        virtual const char* Detail() const override { return "DirectX12 is a windows only graphics API with similar performance to vulkan"; }

        virtual bool RequiresDevice() const override { return true; }

    private:

#if VDXDEBUG
        DX12::ComPtr<DX12::InfoQueue> DebugQueue;
        void EnableDebugger();
#endif

        static constexpr Cthulhu::U8 FrameCount = 2;

        // Pipeline objects
        D3D12_VIEWPORT Viewport;
        D3D12_RECT ScissorRect;
        DX12::ComPtr<DX12::SwapChain> Swap;
        DX12::ComPtr<DX12::Device> Device;
        DX12::ComPtr<DX12::Resource> Targets[FrameCount];
        DX12::ComPtr<DX12::Allocator> Allocators[FrameCount];
        DX12::ComPtr<DX12::Queue> Queue;
        DX12::ComPtr<DX12::RootSignature> RootSignature;
        DX12::ComPtr<DX12::DescriptorHeap> RTVHeap;
        DX12::ComPtr<DX12::PipelineState> PipelineState;
        DX12::ComPtr<DX12::CommandList> CommandList;
        Cthulhu::U32 RTVDescriptorSize;

        // Resource objects
        DX12::ComPtr<DX12::Resource> VertexBuffer;
        D3D12_VERTEX_BUFFER_VIEW VertexBufferView;

        // Synchronization objects
        Cthulhu::U32 FrameIndex;
        DX12::Handle FenceEvent;
        DX12::ComPtr<DX12::Fence> Fence;
        Cthulhu::U64 FenceValues[FrameCount];
    public:
        void Render();
    private:
        void PopulateCommandList();
        void MoveToNextFrame();

        void WaitForGPU();

        Cthulhu::Array<DX12::DX12Device> RenderDevices;

        Frame Frame;
    };
}
