#include "Metal.h"

namespace Volts::Render
{
    static Metal* Singleton = new Metal();
    Metal::Metal()
    {
        Emulator::Get()->Render.Register(this);

        NSArray<id<MTLDevice>>* MTLDevices = MTLCopyAllDevices();

        for(id<MTLDevice> Device in MTLDevices)
            DeviceList.push_back(Device);
    }

    const Device* Metal::Devices(U32* Count) const
    {
        *Count = DeviceList.size();
        return DeviceList.data();
    }
}