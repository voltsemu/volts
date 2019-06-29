#pragma once

#include "Volts/Config.h"

#include "PS3/VM/Render/Render.h"
#include "Volts/Core/Logger/Logger.h"

#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_5.h>
#include <wrl.h>
#include <directxmath.h>

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
    using PipelineState = ID3D12PipelineState;
    using RootSignature = ID3D12RootSignature;
    using Blob = ID3DBlob;

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

    struct Vertex
    {
        XMFLOAT3 Position;
        XMFLOAT4 Colour;
    };

    struct ShaderBytecode : public D3D12_SHADER_BYTECODE
    {
        ShaderBytecode(ComPtr<Blob> Shader)
            : pShaderBytecode(Shader->GetBufferPointer())
            , BytecodeLength(Shader->GetBufferSize())
        {}
    };

    static_assert(sizeof(ShaderBytecode) == sizeof(D3D12_SHADER_BYTECODE));

    struct RasterDesc : public D3D12_RASTERIZER_DESC
    {
        RasterDesc()
            : FillMode(D3D12_FILL_MODE_SOLID)
            , CullMode(D3D12_CULL_MODE_BACK)
            , FrontCounterClockwise(false)
            , DepthBias(D3D12_DEFAULT_DEPTH_BIAS)
            , DepthBiasClamp(D3D12_DEFAULT_DEPTH_BIAS_CLAMP)
            , SlopeScaledDepthBias(D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS)
            , DepthClipEnable(true)
            , MultisampleEnable(false)
            , AntialiasedLineEnable(false)
            , ForcedSampleCount(0)
            , ConservativeRaster(D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF)
        {}
    };

    static_assert(sizeof(RasterDesc) == sizeof(D3D12_RASTERIZER_DESC));

    struct BlendDesc : public D3D12_BLEND_DESC
    {
        BlendDesc()
            : AlphaToCoverageEnable(false)
            , IndependentBlendEnable(false)
        {
            const D3D12_RENDER_TARGET_BLEND_DESC Desc =
            {
                false,
                false,
                D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
                D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
                D3D12_LOGIC_OP_NOOP,
                D3D12_COLOR_WRITE_ENABLE_ALL,
            };

            for(Cthulhu::U32 I = 0; I < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; I++)
                RenderTarget[I] = Desc;
        }
    };

    static_assert(sizeof(BlendDesc) == sizeof(D3D12_BLEND_DESC));

    struct HeapProps : public D3D12_HEAP_PROPERTIES
    {
        HeapProps(D3D12_HEAP_TYPE InType)
            : Type(InType)
            , CPUPageProperty(D3D12_CPU_PAGE_PROPERTY_UNKOWN)
            , MemoryPoolPreference(D3D12_MEMORY_POOL_UNKNOWN)
            , CreationNodeMask(1)
            , VisibleNodeMask(1)
        {}
    };

    static_assert(sizeof(HeapProps) == sizeof(D3D12_HEAP_PROPERTIES));

    struct ResourceDesc : public D3D12_RESOURCE_DESC
    {
        ResourceDesc(
            D3D12_RESOURCE_DIMENSION InDimension,
            UINT64 Align,
            UINT64 InWidth,
            UINT InHeight,
            UINT16 InDepthOrArraySize,
            UINT16 MIPLevels,
            DXGI_FORMAT InFormat,
            UINT SampleCount,
            UINT SampleQuality,
            D3D12_TEXTURE_LAYOUT InLayout,
            D3D12_RESOURCE_FLAGS InFlags
        )
            : Dimension(InDimension)
            , Alignment(Align)
            , Width(InWidth)
            , Height(InHeight)
            , DepthOrArraySize(InDepthOrArraySize)
            , MipLevels(MIPLevels)
            , Format(InFormat)
            , SampleDesc({ SampleCount, SampleQuality })
            , Layout(InLayout)
            , Flags(InFlags)
        {}

        static ResourceDesc Buffer(UINT64 Width, D3D12_RESOURCE_FLAGS Flags = D3D12_RESOURCE_FLAG_NONE, UINT64 Align = 0)
        {
            return ResourceDesc(
                D3D12_RESOURCE_DIMENSION_BUFFER,
                Align, Width,
                1, 1, 1,
                DXGI_FORMAT,
                1, 0,
                D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
                Flags
            );
        }
    };

    static_assert(sizeof(ResourceDesc) == sizeof(D3D12_RESOURCE_DESC));

    struct ResourceBarrier : D3D12_RESOURCE_BARRIER
    {
        static ResourceBarrier Transition(
            Resource* Res,
            D3D12_RESOURCE_STATES Before,
            D3D12_RESOURCE_STATES After
        )
        {
            D3D12_RESOURCE_BARRIER Barrier = {};
            Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            Barrier.Transition = { Res, Before, After, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES };

            return Barrier;
        }
    };

    static_assert(sizeof(ResourceBarrier) == sizeof(D3D12_RESOURCE_BARRIER));

    bool CanTear();

#if VDXDEBUG
#   define DX_CHECK(...) { auto HR = __VA_ARGS__; if(FAILED(HR)) { LOGF_ERROR(DX12, "Check failed " __FILE__ ":" STR(__LINE__) " HR(0x%x) %s", HR, _com_error(HR).ErrorMessage()); } }
#else
#   define DX_CHECK(...) { __VA_ARGS__; }
#endif
}