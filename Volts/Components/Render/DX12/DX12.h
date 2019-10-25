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

        virtual void UpdateVSync(bool Enabled) override;
        virtual void UpdateFullscreen(bool Enabled) override;

        virtual void GUI()
        {
            static bool AAA = false;
            ImGui::Checkbox("AAA", &AAA);

            if(AAA)
            {
                ClearColour[0] = 0.f;
                Viewport = {
                    0.f, 60.f,
                    720, 360
                };
            }
            else
            {
                ClearColour[0] = 1.f;
                Viewport = {
                    0.f, 0.f,
                    720, 380
                };
            }
        }

    private:

        F32 ClearColour[4] = { 1.f, 1.f, 1.f, 1.f };

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

        // creates the device and the command queue
        void CreateDeviceData();

        void LoadDeviceData();

        void LoadSizedResources();

        void LoadData();

        void WaitForGPU();
        void AdvanceFrame();

        void CreateBackBuffer();

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
        bool Fullscreen;
        U32 VSyncMode = 0;
    };
}