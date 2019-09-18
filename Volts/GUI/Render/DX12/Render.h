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
        virtual void Start(GUI::Frame* Handle) override;
        virtual void Detach() override;
        virtual const String& Name() const override { return "DX12"; }
        virtual const String& Description() const override { return "DirectX12"; }

        virtual void InitGUI() const override
        {
            // TODO: info
            ImGui_ImplDX12_Init();
        }

        virtual void NewGUIFrame() const override
        {
            ImGui_ImplDX12_NewFrame();
        }

        virtual void ShutdownGUI() const override
        {
            ImGui_ImplDX12_ShutDown();
        }

        virtual void RenderGUI() const override
        {
            ImDrawData* Draw = ImGui::GetDrawData();
            ImGui_ImplDX12_RenderDrawData(Draw, /* todo */);
        }

    private:
        bool Setup();
        bool LoadPipeline();

        Ptr<IDXGISwapChain3> Swap;
        Ptr<ID3D12Device> Device;
    };
}