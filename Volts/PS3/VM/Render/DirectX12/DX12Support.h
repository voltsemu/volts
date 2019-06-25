#pragma once

#include "PS3/VM/Render/Render.h"

#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_5.h>
#include <wrl.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")

namespace Volts::PS3::RSX::DX12
{
    template<typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    using Device = ID3D12Device5;
    using Queue = ID3D12CommandQueue;
    using SwapChain = IDXGISwapChain4;
    using Resource = ID3D12Resource;
    using CommandList = ID3D12GraphicsCommandList;
    using Allocator = ID3D12CommandAllocator;
    using DescriptorHeap = ID3D12DescriptorHeap;
    using Fence = ID3D12Fence;
    using Debug = ID3D12Debug;

    using Factory = IDXGIFactory1;
    using Adapter = IDXGIAdapter1;
    using AdapterDescriptor = DXGI_ADAPTER_DESC1;

    struct DX12Device : RenderDevice
    {
        DX12Device() {}
        DX12Device(IDXGIAdapter1* Adapt);
        ~DX12Device();
        virtual std::wstring Name() const override;
    private:
        Adapter* Adapter = nullptr;
        AdapterDescriptor Descriptor;
    };

    ComPtr<Device> CreateDevice(ComPtr<Adapter> Adapt);
}