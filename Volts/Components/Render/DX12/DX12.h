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
        bool Tearing;
        U32 VSyncMode;
    };
}