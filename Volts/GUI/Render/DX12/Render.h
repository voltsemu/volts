#pragma once

#include "Render/Render.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <wrl.h>
#include <shellapi.h>

#include "imgui/examples/imgui_impl_dx12.h"

namespace Volts::RSX
{
    template<typename T>
    using Ptr = Microsoft::WRL::ComPtr<T>;

    struct DX12 : Render
    {
        virtual ~DX12() {}
        virtual void Start(GUI::Frame* Handle) override;
        virtual void Detach() override;
        virtual const String Name() const override { return "DX12"; }
        virtual const String Description() const override { return "DirectX12"; }

        virtual void InitGUI() const override
        {
            // TODO
        }

        virtual void NewGUIFrame() const override
        {
            // TODO
        }

        virtual void ShutdownGUI() const override
        {
            // TODO
        }

        virtual void RenderGUI() const override
        {
            // TODO
        }

    private:
        bool Setup();
        bool LoadPipeline();

        Ptr<IDXGISwapChain3> Swap;
        Ptr<ID3D12Device> Device;
    };
}