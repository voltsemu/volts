#pragma once

#include "Volts/Config.h"

#include "PS3/VM/Render/Render.h"
#include "Volts/Core/Logger/Logger.h"

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
    using SwapChain1 = IDXGISwapChain1;
    using Resource = ID3D12Resource;
    using CommandList = ID3D12GraphicsCommandList;
    using Allocator = ID3D12CommandAllocator;
    using DescriptorHeap = ID3D12DescriptorHeap;
    using Fence = ID3D12Fence;
    using Debug = ID3D12Debug;
    using InfoQueue = ID3D12InfoQueue;

    using Factory = IDXGIFactory4;
    using Factory5 = IDXGIFactory5;
    using Adapter = IDXGIAdapter1;

    using Handle = HANDLE;
    using Result = HRESULT;

    using AdapterDescriptor = DXGI_ADAPTER_DESC1;
    using QueueDescriptor = D3D12_COMMAND_QUEUE_DESC;
    using SwapDescriptor = DXGI_SWAP_CHAIN_DESC;
    using HeapDescriptor = D3D12_DESCRIPTOR_HEAP_DESC;

    struct DX12Device : RenderDevice
    {
        DX12Device() {}
        DX12Device(IDXGIAdapter1* Adapt);
        ~DX12Device();
        virtual std::wstring Name() const override;

        Adapter* GetAdapter() const { return Adapter; }
    private:
        Adapter* Adapter = nullptr;
        AdapterDescriptor Descriptor;
    };

    bool CanTear();

#if VDXDEBUG
#   define DX_CHECK(...) { auto HR = __VA_ARGS__; if(FAILED(HR)) { LOGF_ERROR(DX12, "Check failed " __FILE__ ":" STR(__LINE__) " HR(0x%x) %s", HR, _com_error(HR).ErrorMessage()); } }
#else
#   define DX_CHECK(...) { __VA_ARGS__; }
#endif
}