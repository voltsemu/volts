#pragma once

#include "Support.h"
#include <vector>

namespace Volts::Render
{
    struct Metal : Render
    {
        Metal();
        virtual ~Metal() {}
        virtual const char* Name() const override { return "Metal"; }

        virtual const Device* Devices(U32* Count) const override;

    private:
        std::vector<MetalDevice> DeviceList;
    };
}