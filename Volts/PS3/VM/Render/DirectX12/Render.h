#pragma once

#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_5.h>
#include <wrl.h>

#include <Core/Collections/Array.h>
#include "PS3/VM/Render/Render.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")

namespace Volts::PS3::RSX
{
    namespace DX12
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

        using Factory = IDXGIFactory1;
        using Adapter = IDXGIAdapter1;
        using AdapterDescriptor = DXGI_ADAPTER_DESC1;

        using DLL = HMODULE;
    }

    struct DX12Device : RenderDevice
    {
        DX12Device() {}
        DX12Device(IDXGIAdapter1* Adapt);
        ~DX12Device();
        virtual const char* Name() const override;
    private:
        DX12::Adapter* Adapter = nullptr;
        DX12::AdapterDescriptor* Descriptor = nullptr;
    };

    struct DirectX12 : Render
    {
        DirectX12();
        virtual ~DirectX12() override {}

        virtual InitError Init() override;
        virtual void DeInit() override;
        virtual bool Supported() const override;

        virtual RenderDevice* Devices(unsigned& Count) const override;

        virtual const char* Name() const override { return "DirectX12"; }
        virtual const char* Detail() const override { return "DirectX12 is a windows only graphics API with similar performance to vulkan"; }
    private:
        DX12::DLL Library;
        Cthulhu::Array<DX12Device> RenderDevices;
    };
}
