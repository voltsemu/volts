#include "Support.h"

namespace Volts::Render
{
    struct DX12 : Render
    {
        DX12();
        virtual ~DX12() override {}

        virtual const char* Name() const override { return "DirectX12"; }

        virtual const Device* Devices(U32* Count) const
        {
            *Count = DeviceList.size();
            return DeviceList.data();
        }

        virtual void SetDevice(U32 Index) override;

        virtual void Attach() override;
        virtual void Detach() override;

        virtual void Begin() override;
        virtual void End() override;

    private:
        // global state
        Ptr<IDXGIFactory4> Factory;
        std::vector<DX12Device> DeviceList;
        U32 DeviceIndex = 0;

        // helper functions
        void LoadPipeline();
        void LoadData();

        static constexpr U32 FrameCount = 2;

        // pipeline state
        D3D12_VIEWPORT Viewport;
        D3D12_RECT Scissor;

        Ptr<IDXGISwapChain3> Swap;
        Ptr<ID3D12Device> Device;
        Ptr<ID3D12CommandQueue> Queue;

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

        // synchronization data
        U32 FrameIndex;
        HANDLE FenceEvent;
        Ptr<ID3D12Fence> Fence;
        U64 FenceValues[FrameCount];

        // extra data
        bool Tearing;
        U32 VSyncMode;
    };
}