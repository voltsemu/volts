#pragma once

#include "DX12Support.h"
#include "PS3/VM/Render/Frame.h"

#include <Core/Collections/Array.h>

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

        void SetupSwapChain();
        void SetupHeap();
        void SetupDevice(DX12::Adapter* Adapt);
        void SetupList(DX12::Allocator* Alloc);
        void SetupFence();
        void SetupFenceHandle();

        DX12::ComPtr<DX12::Allocator> CreateAllocator();

        void UpdateRTV();
        void Update();
        void Render();

        Cthulhu::U64 Signal(Cthulhu::U64& FenceValue);
        void WaitForValue(Cthulhu::U64 Duration);
        void Flush(Cthulhu::U64& FenceValue);

        static constexpr Cthulhu::U8 SwapFrames = 3;
        Cthulhu::U64 FrameCounter = 0;

        bool Initialized = false;
        bool VSync = false;
        bool CanTear = false;
#if VDXDEBUG
        DX12::ComPtr<DX12::Debug> Debugger;
        DX12::ComPtr<DX12::InfoQueue> DebugQueue;
#endif
        DX12::ComPtr<DX12::Device> CurrentDevice;
        DX12::ComPtr<DX12::DescriptorHeap> Heap;
        DX12::ComPtr<DX12::SwapChain> Swap;
        DX12::ComPtr<DX12::Queue> Queue;
        DX12::ComPtr<DX12::CommandList> CommandList;

        Cthulhu::U64 FenceVal = 0;
        DX12::ComPtr<DX12::Fence> Fence;
        DX12::Handle FenceEvent;

        Cthulhu::U64 CurrentBackBuffer;
        Cthulhu::U64 RTVSize;

        Cthulhu::U64 FenceValues[SwapFrames] = {};
        DX12::ComPtr<DX12::Resource> Buffers[SwapFrames];
        DX12::ComPtr<DX12::Allocator> Allocators[SwapFrames];

        Cthulhu::Array<DX12::DX12Device> RenderDevices;
        Frame Frame;
    };
}
