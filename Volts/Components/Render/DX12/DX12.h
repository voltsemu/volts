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

        virtual void Resize(U32 Width, U32 Height) override;

    private:

        struct Vertex
        {
            F32 Pos[3];
            F32 Colour[4];
        };
        // global state
        Ptr<IDXGIFactory4> Factory;
        std::vector<DX12Device> DeviceList;
        U32 DeviceIndex = 0;
        IDXGIAdapter1* CurrentDevice() const { return DeviceList[DeviceIndex].Handle; }

        // load data that is independant of device or framesize
        void LoadData();

        // load data that is dependant on device & framesize
        void LoadDeviceData();

        // reset state that is frame size dependant
        void ResetData();

        // wait for gpu to do its stuff
        void WaitForGPU();

        // go to the next frame
        void AdvanceFrame();

        // add the stuff we need to the command list
        void PopulateCommandList();

        static constexpr U32 FrameCount = 2;

        // debug
        Ptr<ID3D12InfoQueue> DebugQueue;

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
        U32 VSyncMode = 0;
    };
}